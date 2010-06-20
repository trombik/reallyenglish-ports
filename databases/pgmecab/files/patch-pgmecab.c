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
 
-/* 空文字を返す関数 */
+/* ??文?????屬??愎? */
 static Datum returnEmptyStr()
 {
 	text* emptyVal = (text*) palloc(VARHDRSZ);
-	VARATT_SIZEP(emptyVal) = VARHDRSZ;
+	SET_VARLENA_LEN(emptyVal, VARHDRSZ);
 	PG_RETURN_TEXT_P(emptyVal);
 }
 
 /**
- * mecabの処理結果チェックマクロ
- * 出来れば、エラー時はPostgreSQLのエラーへ
- * 出力したいところなのだが、出力の方法が分からず・・・
+ * mecab?僚??????魅船??奪??泪???
+ * ???????弌????蕁�????PostgreSQL?離??蕁�??
+ * ???呂??????箸????覆里世??????呂???法??分???蕕�??????
  */
 #define CHECK(eval) if (! eval) { \
 	mecab_destroy(mecab); \
@@ -38,19 +40,19 @@
 
 Datum pgmecab(PG_FUNCTION_ARGS)
 {
-	/* NULLが来たらNULLを返す */
+	/* NULL???茲�??NULL???屬? */
 	if(PG_ARGISNULL(0))
 	{
 		PG_RETURN_NULL();
 	}
 	
-	/* 引数を取得 */
+	/* ?????????? */
 	text* input_words = PG_GETARG_TEXT_P(0);
 	
-	/* 実際の入力文字列のサイズにNULLターミネート分の+1をする */
+	/* ?尊櫃?????文?????離???????NULL?????潺諭???分??+1?鬚垢? */
 	size_t input_size = VARSIZE(input_words)-VARHDRSZ+1;
 	
-	/* 入力文字列が空だったら空文字を返す */
+	/* ????文???鵑��??世辰?????文?????屬? */
 	if(input_size <= 1)
 	{
 		returnEmptyStr();
@@ -59,17 +61,17 @@
 	char* search_words = NULL;
 	
 	/*
-	 * 入力文字列用メモリ確保
-	 * palloc0は、0で初期化されたメモリをくれる
+	 * ????文?????僖?????????
+	 * palloc0?蓮?0?能??�化された???????鬚��???
 	 */
 	search_words = (char*)palloc0(input_size * sizeof(char));
 	
-	/* 文字をコピー */
+	/* 文???鬟灰圈? */
 	memcpy(search_words, VARDATA(input_words), input_size-1);
 	
 	/*
-	 * mecabの初期化
-	 * 分かち書きモードを設定
+	 * mecab?僚?????
+	 * 分???曾颪�モー?匹?????
 	 */
 	const int mecav_argc = 3;
 	char* mecav_argv[] = {"mecab", "-O", "wakati"};
@@ -79,21 +81,21 @@
 	const char* mecab_result = mecab_sparse_tostr(mecab, search_words);
 	CHECK(mecab_result);
 	
-	/* 入力に使ったメモリ解放 */
+	/* ???呂忙箸辰??????????? */
 	pfree(search_words);
 	search_words = NULL;
 	
-	/* 結果をコピー */
+	/* ???未鬟灰圈? */
 	size_t mecab_result_size = strlen(mecab_result);
 	
-	/* 返すためのメモリ確保 */
+	/* ?屬??????離????????? */
 	text* return_val = (text *) palloc(VARHDRSZ + mecab_result_size * sizeof(char));
-	VARATT_SIZEP(return_val) = VARHDRSZ + mecab_result_size * sizeof(char);
+	SET_VARLENA_LEN(return_val, VARHDRSZ + mecab_result_size * sizeof(char));
 	
-	/* 結果をコピー */
+	/* ???未鬟灰圈? */
 	memcpy((void*)VARDATA(return_val), mecab_result, mecab_result_size);
 	
-	/* MeCabの後始末 */
+	/* MeCab?慮????? */
 	mecab_destroy(mecab);
 	
 	PG_RETURN_TEXT_P(return_val);
