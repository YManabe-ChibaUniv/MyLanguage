- 構文木：https://www.jstage.jst.go.jp/article/konpyutariyoukyouiku/36/0/36_56/_pdf/-char/ja
- gdb 操作：https://www.tohoho-web.com/ex/draft/gdb.htm
- 呼び出し元の関数：https://www.techiedelight.com/ja/find-name-of-the-calling-function-in-cpp/
- enum：https://kenkyu-note.hatenablog.com/entry/2020/07/06/195821
- 全体：http://web.sfc.keio.ac.jp/~hagino/sa15/05-handout.pdf
- コードリスト：https://www.fit.ac.jp/~m-ishihara/Lectures/H22/Compiler/12/Handout.pdf


### 結果 10000までの素数の計算
- MyLanguage
  - vector<char> (runtime_file): 1.03998s
  - char[] (runtime_file): 0.825894s
  - static stack (std::stack<Var*> -> VarStack): 0.624698s
- C++ (O3): 0.014539s
- Python: 0.521736s
