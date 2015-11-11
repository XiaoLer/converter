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
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#include "php_converter.h"

/* If you declare any globals in php_converter.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(converter)
*/

/* True global resources - no need for thread safety here */
static int le_converter;


/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_str_convert, 0, 0, 0)
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

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("converter.enabled",   "1", PHP_INI_ALL, OnUpdateLong, global_value, zend_converter_globals, converter_globals)
    STD_PHP_INI_ENTRY("converter.dictionary", "", PHP_INI_ALL, OnUpdateString, global_string, zend_converter_globals, converter_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_converter_init_globals
 */
static void php_converter_init_globals(zend_converter_globals *converter_globals)
{
	converter_globals->enabled	  = 0;
	converter_globals->dictionary = NULL;
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

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(converter)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(converter)
{
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

/* string convert */
PHP_FUNCTION(str_convert) /* {{{ */
{
	char *string = NULL;
	int str_len;
	char *converted;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &str_len) == FAILURE) {
		return;
	}

	// RETURN_STRING(converted, 0);
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
