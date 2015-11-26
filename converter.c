/*
  +----------------------------------------------------------------------+
  | PHP Output Converter                                                 |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Scholer Liu <scholer_l@live.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/php_output.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#include "php_converter.h"

ZEND_DECLARE_MODULE_GLOBALS(converter)

/* True global resources - no need for thread safety here */
static int le_converter;


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_str_convert, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()
/* }}} */


/* {{{ converter_functions[]
 *
 * Every user visible function must have an entry in converter_functions[].
 */
const zend_function_entry converter_functions[] = {
	PHP_FE(str_convert, arginfo_str_convert)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in converter_functions[] */
};
/* }}} */

/* {{{ converter_module_entry
 */
zend_module_entry converter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"converter",
	converter_functions,
	PHP_MINIT(converter),
	PHP_MSHUTDOWN(converter),
	PHP_RINIT(converter),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(converter),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(converter),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_CONVERTER_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CONVERTER
ZEND_GET_MODULE(converter)
#endif

static int converter_dictionary_load(TSRMLS_D);
static int conveter_str_convert(char *string, zval *str_converted TSRMLS_DC);
static int converter_output_handler(void **handler_context, php_output_context *output_context);

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("converter.auto_convert", "1", PHP_INI_ALL, OnUpdateLong, auto_convert, zend_converter_globals, converter_globals)
    STD_PHP_INI_ENTRY("converter.dictionary",    "", PHP_INI_ALL, OnUpdateString, dictionary, zend_converter_globals, converter_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_converter_init_globals
 */
static void php_converter_init_globals(zend_converter_globals *converter_globals)
{
	converter_globals->auto_convert = 0;
	converter_globals->dictionary   = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(converter)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(converter)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(converter)
{
	php_output_handler *handler;
	const char conveter_handler[] = "converter handler";

	if (converter_dictionary_load(TSRMLS_C) != SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Can not read dictionary file.");
	}

	if (CONVERTER_G(auto_convert) == 1) {
		handler = php_output_handler_create_internal(ZEND_STRL(conveter_handler), converter_output_handler, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC);
		if (SUCCESS == php_output_handler_start(handler TSRMLS_CC)) {
			return SUCCESS;
		}
		php_output_handler_free(&handler TSRMLS_CC);
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(converter)
{
	efree(CONVERTER_G(search));
	efree(CONVERTER_G(replace));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(converter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "converter support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static int converter_output_handler(void **handler_context, php_output_context *output_context)
{
	zval *str_converted = NULL;

	MAKE_STD_ZVAL(str_converted);

	if(SUCCESS == conveter_str_convert(output_context->in.data, str_converted TSRMLS_DC)) {
		output_context->out.data = estrndup(Z_STRVAL_P(str_converted), Z_STRLEN_P(str_converted));
		output_context->out.used = Z_STRLEN_P(str_converted);
		output_context->out.free = 1;
	}

	zval_dtor(str_converted);

	return SUCCESS;
}

/* {{{ int converter_dictionary_load(TSRMLS_D)
 * load dictonary and parsing */
static int converter_dictionary_load(TSRMLS_D)
{
	char *delim = "|";
	zval zdelim, zstr;
	php_stream *stream;

	MAKE_STD_ZVAL(CONVERTER_G(search));
	MAKE_STD_ZVAL(CONVERTER_G(replace));

	array_init(CONVERTER_G(search));
	array_init(CONVERTER_G(replace));

	stream = php_stream_open_wrapper(CONVERTER_G(dictionary), "r", USE_PATH, NULL);
	if (!stream) {
		return FAILURE;
	}

	ZVAL_STRINGL(&zdelim, delim, strlen(delim), 1);

	while (!php_stream_eof(stream)) {
		char *line = NULL, *line_trimed = NULL;
		zval *splited;

		zval **search_entry, **replace_entry;

		line = php_stream_gets(stream, NULL, 1024);
		if (line == NULL) {
			continue;
		}
		line_trimed = php_trim(line, strlen(line), NULL, 0, NULL, 2 TSRMLS_DC);
		efree(line);

		ZVAL_STRING(&zstr, line_trimed, 1);
		efree(line_trimed);

		MAKE_STD_ZVAL(splited);
		array_init(splited);

		php_explode(&zdelim, &zstr, splited, LONG_MAX);

		if (zend_hash_index_find(Z_ARRVAL_P(splited), 0, (void *)&search_entry) == SUCCESS
			&& zend_hash_index_find(Z_ARRVAL_P(splited), 1, (void *)&replace_entry) == SUCCESS)
		{
			SEPARATE_ZVAL(search_entry);
			SEPARATE_ZVAL(replace_entry);

			add_next_index_string(CONVERTER_G(search), Z_STRVAL_PP(search_entry), 0);
			add_next_index_string(CONVERTER_G(replace), Z_STRVAL_PP(replace_entry), 0);
		}

		efree(splited);
	}

	php_stream_close(stream);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_FUNCTION(str_convert)
 * function string convert */
PHP_FUNCTION(str_convert)
{
	char *string = NULL;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &str_len) == FAILURE) {
		return;
	}

	if (conveter_str_convert(string, return_value TSRMLS_CC) == FAILURE) {
		RETURN_STRING(string, 1);
	}

	if (Z_TYPE_P(return_value) == IS_STRING) {
		RETURN_STRING(Z_STRVAL_P(return_value), 1);
	} else {
		RETURN_STRING(string, 1);
	}
}
/* }}} */

/* {{{ int conveter_str_convert
 * convert string */
static int conveter_str_convert(char *string, zval *str_converted TSRMLS_DC)
{
	zval *zstring;

	zval *params[3] = {0};
	zval function = {{0}, 0};

	MAKE_STD_ZVAL(zstring);
	ZVAL_STRING(zstring, string, 0);

	params[0] = CONVERTER_G(search);
	params[1] = CONVERTER_G(replace);
	params[2] = zstring;

	ZVAL_STRING(&function, "str_replace", 0);

	if (call_user_function(EG(function_table), NULL, &function, str_converted, 3, params TSRMLS_CC) == FAILURE) {
		if (str_converted) {
			zval_dtor(str_converted);
		}
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Call to str_replace failed");
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
