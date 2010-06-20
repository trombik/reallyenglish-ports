--- ./pgmecab.c.orig	2006-02-15 00:36:39.000000000 +0900
+++ ./pgmecab.c	2010-06-20 06:30:19.000000000 +0900
@@ -7,7 +7,9 @@
 
 #include "postgres.h"
 #include "fmgr.h"
-
+#ifdef PG_MODULE_MAGIC
+PG_MODULE_MAGIC;
+#endif
 #include "mecab.h"
 
 #include <stdio.h>
@@ -18,18 +20,18 @@
 
 extern Datum pgmecab(PG_FUNCTION_ARGS);
 
-/* ¶õÊ¸»ú¤òÊÖ¤¹´Ø¿ô */
+/* ??Ê¸?????Ö¤??Ø¿? */
 static Datum returnEmptyStr()
 {
 	text* emptyVal = (text*) palloc(VARHDRSZ);
-	VARATT_SIZEP(emptyVal) = VARHDRSZ;
+	SET_VARLENA_LEN(emptyVal, VARHDRSZ);
 	PG_RETURN_TEXT_P(emptyVal);
 }
 
 /**
- * mecab¤Î½èÍý·ë²Ì¥Á¥§¥Ã¥¯¥Þ¥¯¥í
- * ½ÐÍè¤ì¤Ð¡¢¥¨¥é¡¼»þ¤ÏPostgreSQL¤Î¥¨¥é¡¼¤Ø
- * ½ÐÎÏ¤·¤¿¤¤¤È¤³¤í¤Ê¤Î¤À¤¬¡¢½ÐÎÏ¤ÎÊýË¡¤¬Ê¬¤«¤é¤º¡¦¡¦¡¦
+ * mecab?Î½??????Ì¥Á¥??Ã¥??Þ¥???
+ * ???????Ð¡????é¡¼????PostgreSQL?Î¥??é¡¼??
+ * ???Ï¤??????È¤????Ê¤Î¤À¤??????Ï¤???Ë¡??Ê¬???é¤º??????
  */
 #define CHECK(eval) if (! eval) { \
 	mecab_destroy(mecab); \
@@ -38,19 +40,19 @@
 
 Datum pgmecab(PG_FUNCTION_ARGS)
 {
-	/* NULL¤¬Íè¤¿¤éNULL¤òÊÖ¤¹ */
+	/* NULL???è¤¿??NULL???Ö¤? */
 	if(PG_ARGISNULL(0))
 	{
 		PG_RETURN_NULL();
 	}
 	
-	/* °ú¿ô¤ò¼èÆÀ */
+	/* ?????????? */
 	text* input_words = PG_GETARG_TEXT_P(0);
 	
-	/* ¼ÂºÝ¤ÎÆþÎÏÊ¸»úÎó¤Î¥µ¥¤¥º¤ËNULL¥¿¡¼¥ß¥Í¡¼¥ÈÊ¬¤Î+1¤ò¤¹¤ë */
+	/* ?ÂºÝ¤?????Ê¸?????Î¥???????NULL?????ß¥Í¡???Ê¬??+1?ò¤¹¤? */
 	size_t input_size = VARSIZE(input_words)-VARHDRSZ+1;
 	
-	/* ÆþÎÏÊ¸»úÎó¤¬¶õ¤À¤Ã¤¿¤é¶õÊ¸»ú¤òÊÖ¤¹ */
+	/* ????Ê¸???ó¤¬¶??À¤Ã¤?????Ê¸?????Ö¤? */
 	if(input_size <= 1)
 	{
 		returnEmptyStr();
@@ -59,17 +61,17 @@
 	char* search_words = NULL;
 	
 	/*
-	 * ÆþÎÏÊ¸»úÎóÍÑ¥á¥â¥ê³ÎÊÝ
-	 * palloc0¤Ï¡¢0¤Ç½é´ü²½¤µ¤ì¤¿¥á¥â¥ê¤ò¤¯¤ì¤ë
+	 * ????Ê¸?????Ñ¥?????????
+	 * palloc0?Ï¡?0?Ç½??ü²½¤µ¤ì¤¿???????ò¤¯¤???
 	 */
 	search_words = (char*)palloc0(input_size * sizeof(char));
 	
-	/* Ê¸»ú¤ò¥³¥Ô¡¼ */
+	/* Ê¸???ò¥³¥Ô¡? */
 	memcpy(search_words, VARDATA(input_words), input_size-1);
 	
 	/*
-	 * mecab¤Î½é´ü²½
-	 * Ê¬¤«¤Á½ñ¤­¥â¡¼¥É¤òÀßÄê
+	 * mecab?Î½?????
+	 * Ê¬???Á½ñ¤­¥â¡¼?É¤?????
 	 */
 	const int mecav_argc = 3;
 	char* mecav_argv[] = {"mecab", "-O", "wakati"};
@@ -79,21 +81,21 @@
 	const char* mecab_result = mecab_sparse_tostr(mecab, search_words);
 	CHECK(mecab_result);
 	
-	/* ÆþÎÏ¤Ë»È¤Ã¤¿¥á¥â¥ê²òÊü */
+	/* ???Ï¤Ë»È¤Ã¤??????????? */
 	pfree(search_words);
 	search_words = NULL;
 	
-	/* ·ë²Ì¤ò¥³¥Ô¡¼ */
+	/* ???Ì¤ò¥³¥Ô¡? */
 	size_t mecab_result_size = strlen(mecab_result);
 	
-	/* ÊÖ¤¹¤¿¤á¤Î¥á¥â¥ê³ÎÊÝ */
+	/* ?Ö¤??????Î¥????????? */
 	text* return_val = (text *) palloc(VARHDRSZ + mecab_result_size * sizeof(char));
-	VARATT_SIZEP(return_val) = VARHDRSZ + mecab_result_size * sizeof(char);
+	SET_VARLENA_LEN(return_val, VARHDRSZ + mecab_result_size * sizeof(char));
 	
-	/* ·ë²Ì¤ò¥³¥Ô¡¼ */
+	/* ???Ì¤ò¥³¥Ô¡? */
 	memcpy((void*)VARDATA(return_val), mecab_result, mecab_result_size);
 	
-	/* MeCab¤Î¸å»ÏËö */
+	/* MeCab?Î¸????? */
 	mecab_destroy(mecab);
 	
 	PG_RETURN_TEXT_P(return_val);
