# 概要
正規表現で記述された規則を基に直接コード化された字句解析器を出力する字句解析器生成系です。

# 開発状況
1. エンジン（済）
2. 出力形式の規定（済）
3. 入力形式の規定（済）
4. エラー処理（済）
5. 安定版リリースに向けた最終調整

# 使用言語・ツールと動作確認
<table>
<tr><td>Language</td><td>C++11 (Clang++ 3.2 / VC++ 10.0)</td></tr>
<tr><td>Tool</td><td>kp19pp Ver. Jun 03, 2012 (<a href="https://github.com/uwanosora/kp19pp">https://github.com/uwanosora/kp19pp</a>)</td></tr>
</table>

# 使い方
## 実行ファイルの生成
"paper_cutter.cpp"を上記のコンパイラもしくは上記と互換性のあるコンパイラでコンパイルします。

## lexerの生成
    paper_cutter -c++ [-indent=space | -indent=space4 | -indent=space8 | -indent=tab] ifile.txt ofile.hpp

## 入力ファイル
以下の様な形になります。

    namespace_lexer
    reg_1 = regular-expression
    reg_2 = regular-expression
    ...
    reg_n = regular-expression
    skip_1 != regular-expression
    skip_2 != regular-expression
    ...
    skip_n != regular-expression
`namespace_lexer`は、出力されるlexerの名前空間になります。  
先頭がアルファベット或いはアンダースコアで始まり、それ以降がアルファベット、アンダースコア、数字で成り立っている文字列であれば何でも構いません。また、名前空間が記述される前に空白文字（スペース、タブ）が一つ以上存在してもそれらはスキップされます。更に、名前空間の後に空白を挟んで任意の文字列を記述してもそれらは無視されるため、入力の問題になりません。  
`reg_1`から`reg_n`、`skip_1`から`skip_n`はそれぞれ正規表現に関連付けられた名前を示す文字列です。これらに関しても、`namespace_lexer`と同様の書式で記述を行います。  
`regular-expression`には正規表現を記述します。マッチングは上の正規表現から順に評価されます。マッチ結果が保存される`=`の式とマッチ結果が保存されない`!=`の式はどの様な順序で記述しても構いません。
`=`で結び付けられた正規表現はtokenizeメンバ関数（後述：『出力ファイル』）での試行時にマッチした場合、結果は保存されますが、`!=`で結び付けられた正規表現はマッチングのみを行い結果は保存されず次にマッチする条件へとスキップされます。

## 正規表現
<table>
<tr><td>（優先順位1位）</td><td><hr/></td></tr>
<tr><td>a</td><td>文字"a"にマッチ。</td></tr>
<tr><td>.</td><td>ドット。任意の文字にマッチ。</td></tr>
<tr><td>[abc]</td><td>セット。"a"、"b"、"c"のいずれかの一文字にマッチ。</td></tr>
<tr><td>[a-z]</td><td>セット（範囲）。文字コード"a"から"z"の間にあるいずれかの一文字にマッチ。セットと混在可能。<br/>コンパイルされた環境によっては"a", "b", "c", ..., "z"と連続していない点に注意。</td></tr>
<tr><td>[^abc]</td><td>ネガティブセット。"a"、"b"、"c"のいずれかでない一文字にマッチ。</td></tr>
<tr><td>[^a-z]</td><td>ネガティブセット（範囲）。"a"から"z"のいずれかでない一文字にマッチ。ネガティブセットと混在可能。</td></tr>
<tr><td>a*</td><td>アスタリスク。0個以上のaにマッチ。aは何らかの正規表現。</td></tr>
<tr><td>a+</td><td>プラス。1個以上のaにマッチ。</td></tr>
<tr><td>a?</td><td>クエスチョン。0個もしくは1個のaにマッチ。</td></tr>
<tr><td>a{n,m}</td><td>n個以上m個以下のaにマッチ。n、mは数値。</td></tr>
<tr><td>a{n,}</td><td>n個以上のaにマッチ。</td></tr>
<tr><td>a{m}</td><td>m個のaにマッチ。</td></tr>
<tr><td>{<i>other-regular-expression</i>}</td><td>他の正規表現規則`other-regular-expression`にマッチ。</td></tr>
<tr><td>"abc\\xyz\"pqr"</td><td>文字列abc\xyz"pqrにマッチ。</td></tr>
<tr><td>\N</td><td>Nが"a"、'b"、"f"、"n"、"r"、"t"、"v", "\\"の時、"\a"、'\b"、"\f"、"\n"、"\r"、"\t"、"\v"、"\\"に変換されマッチします。<br/>Nがパターン中の演算子の場合、その文字そのものの効果を無効にし、マッチします。</td></tr>
<tr><td>(a)</td><td>aにマッチ。優先順位が1位に変更されます。</td></tr>
<tr><td>（優先順位2位）</td><td><hr/></td></tr>
<tr><td>ab</td><td>正規表現aとbの連結にマッチ。</td></tr>
<tr><td>（優先順位3位）</td><td><hr/></td></tr>
<tr><td>a|b</td><td>ユニオン。aもしくはbにマッチ。</td></tr>
<tr><td>（優先順位4位）</td><td><hr/></td></tr>
<tr><td>a/b</td><td>右文脈。後ろにbが続く時のaにマッチ（参照：右文脈で定義不可能なパターン）。</td></tr>
<tr><td>^a</td><td>ハット。aが行頭（改行の直後もしくはマッチング対象のシーケンスの先頭）にある場合マッチ。</td></tr>
<tr><td>a$</td><td>エンドオブライン。aの次が行端（改行の直前もしくはマッチング対象のシーケンスの終端）にある場合にマッチ。</td></tr>
</table>

## エスケープにおける注意事項
代表的な字句解析プログラムFlexなどと違い、このプログラムでは全ての演算子に対してそのものをマッチさせる場合、どの場所にあろうとも"\"によるエスケープが必要となります。

## 文字クラス
このプログラムでは、以下の文字クラスが使用可能です。

    [:alnum:]
    [:alpha:]
    [:blank:]
    [:cntrl:]
    [:digit:]
    [:graph:]
    [:lower:]
    [:print:]
    [:punct:]
    [:space:]
    [:upper:]
    [:xdigit:]
それぞれC言語のisXXX関数に相当する文字クラスにマッチします。

## 出力ファイル
出力されたファイルは以下のヘッダをincludeします。  

    <utility>
    <iterator>
    <cstring>
    <cctype>
    <ctype.h>
cctypeとctypeの両方をincludeしているのはMSVC 10.0において`std::isblank`が利用できないためです。  
まずは例の入力を見てみましょう。

    aaa
      whitespace   != " "*
      token        =  [a-zA-Z][a-zA-Z0-9]*
      value        =  ([1-9][0-9]*)|0
namespace aaa内に以下のコードが生成されます。  

    namespace aaa{
        enum token{
            token_whitespace,
            token_token,
            token_value
        };

        class lexer{
        public:
            template<class InputIter>
            static std::pair<bool, InputIter> reg_whitespace(InputIter first, InputIter last){ /* 省略 */ }

            template<class InputIter>
            static std::pair<bool, InputIter> reg_token(InputIter first, InputIter last){ /* 省略 */ }

            template<class InputIter>
            static std::pair<bool, InputIter> reg_value(InputIter first, InputIter last){ /* 省略 */ }

            template<class InputIter, class InsertIter>
            static std::pair<bool, InputIter> tokenize(InputIter first, InputIter last, InsertIter token_inserter){
                InputIter iter = first;
                std::pair<bool, InputIter> result;
                while(iter != last){
                    result = reg_whitespace(iter, last);
                    if(result.first){
                        iter = result.second;
                        continue;
                    }
                    result = reg_token(iter, last);
                    if(result.first){
                        *token_inserter = std::make_pair(token_token, std::make_pair(iter, result.second));
                        iter = result.second;
                        continue;
                    }
                    result = reg_value(iter, last);
                    if(result.first){
                        *token_inserter = std::make_pair(token_value, std::make_pair(iter, result.second));
                        iter = result.second;
                        continue;
                    }
                    break;
                }
                return result;
            }
        };
    }
`aaa::token`は、正規表現にマッチした時にtokenizerメンバ関数からinsert\_iteratorを介して出力されるトークンのenumです。  
`aaa::lexer::reg_whitespace`、`aaa::lexer::reg_token`、`aaa::lexer::reg_value`はそれぞれ入力ファイルの正規表現から生成されたメンバ関数です。  
input\_iteratorの入力からマッチするかどうかを判定し、マッチした場合はfirstにtrueを、secondにマッチした直後のiteratorを返します。  
`aaa::lexer::tokenize`メンバ関数内で使用されていますが、publicでstaticなメンバ関数なので単体で使用する事も可能です。  
`aaa::lexer::tokenize`メンバ関数はinput\_iteratorの始端と終端から、現在存在する正規表現全てに対してマッチするかどうかを試行します。トークンを分割した結果は、第3引数のinsert\_iteratorに`std::make_pair(aaa::token, std::make_pair(first, last))`として追加されます。戻り値は`aaa::lexer::reg_XXX`と同じで、何らかの正規表現にマッチした場合firstにtrueが、そうでない場合はfalseが代入され、secondでは走査の終了した位置のiteratorが返されます。

# その他
出力されたデータはそのデータの基となった入力データを作成した本人のものとなります。  
Copyright (c) 2012 uwanosora All Rights Reserved.
