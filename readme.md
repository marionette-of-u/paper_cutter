# �T�v
���K�\���ŋL�q���ꂽ�K������ɒ��ڃR�[�h�����ꂽ�����͊���o�͂��鎚���͊퐶���n�ł��B

# �J����
1. �G���W���i�ρj
2. �o�͌`���̋K��i�ρj
3. ���͌`���̋K��i�ρj
4. �G���[�����i�ρj
5. ����Ń����[�X�Ɍ������ŏI����

# �g�p����E�c�[���Ɠ���m�F
<table>
<tr><td>Language</td><td>C++11 (Clang++ 3.2 / VC++ 10.0)</td></tr>
<tr><td>Tool</td><td>kp19pp Ver. Jun 03, 2012 (<a href="https://github.com/uwanosora/kp19pp">https://github.com/uwanosora/kp19pp</a>)</td></tr>
</table>

# �g����
## ���s�t�@�C���̐���
"paper_cutter.cpp"����L�̃R���p�C���������͏�L�ƌ݊����̂���R���p�C���ŃR���p�C�����܂��B

## lexer�̐���
    paper_cutter -c++ [-indent=space | -indent=space4 | -indent=space8 | -indent=tab] ifile.txt ofile.hpp

## ���̓t�@�C��
�ȉ��̗l�Ȍ`�ɂȂ�܂��B

    namespace_lexer
    reg_1 = regular-expression
    reg_2 = regular-expression
    ...
    reg_n = regular-expression
    skip_1 != regular-expression
    skip_2 != regular-expression
    ...
    skip_n != regular-expression
`namespace_lexer`�́A�o�͂����lexer�̖��O��ԂɂȂ�܂��B  
�擪���A���t�@�x�b�g�����̓A���_�[�X�R�A�Ŏn�܂�A����ȍ~���A���t�@�x�b�g�A�A���_�[�X�R�A�A�����Ő��藧���Ă��镶����ł���Ή��ł��\���܂���B�܂��A���O��Ԃ��L�q�����O�ɋ󔒕����i�X�y�[�X�A�^�u�j����ȏ㑶�݂��Ă������̓X�L�b�v����܂��B�X�ɁA���O��Ԃ̌�ɋ󔒂�����ŔC�ӂ̕�������L�q���Ă������͖�������邽�߁A���̖͂��ɂȂ�܂���B  
`reg_1`����`reg_n`�A`skip_1`����`skip_n`�͂��ꂼ�ꐳ�K�\���Ɋ֘A�t����ꂽ���O������������ł��B�����Ɋւ��Ă��A`namespace_lexer`�Ɠ��l�̏����ŋL�q���s���܂��B  
`regular-expression`�ɂ͐��K�\�����L�q���܂��B�}�b�`���O�͏�̐��K�\�����珇�ɕ]������܂��B�}�b�`���ʂ��ۑ������`=`�̎��ƃ}�b�`���ʂ��ۑ�����Ȃ�`!=`�̎��͂ǂ̗l�ȏ����ŋL�q���Ă��\���܂���B
`=`�Ō��ѕt����ꂽ���K�\����tokenize�����o�֐��i��q�F�w�o�̓t�@�C���x�j�ł̎��s���Ƀ}�b�`�����ꍇ�A���ʂ͕ۑ�����܂����A`!=`�Ō��ѕt����ꂽ���K�\���̓}�b�`���O�݂̂��s�����ʂ͕ۑ����ꂸ���Ƀ}�b�`��������ւƃX�L�b�v����܂��B

## ���K�\��
<table>
<tr><td>�i�D�揇��1�ʁj</td><td><hr/></td></tr>
<tr><td>a</td><td>����"a"�Ƀ}�b�`�B</td></tr>
<tr><td>.</td><td>�h�b�g�B�C�ӂ̕����Ƀ}�b�`�B</td></tr>
<tr><td>[abc]</td><td>�Z�b�g�B"a"�A"b"�A"c"�̂����ꂩ�̈ꕶ���Ƀ}�b�`�B</td></tr>
<tr><td>[a-z]</td><td>�Z�b�g�i�͈́j�B�����R�[�h"a"����"z"�̊Ԃɂ��邢���ꂩ�̈ꕶ���Ƀ}�b�`�B�Z�b�g�ƍ��݉\�B<br/>�R���p�C�����ꂽ���ɂ���Ă�"a", "b", "c", ..., "z"�ƘA�����Ă��Ȃ��_�ɒ��ӁB</td></tr>
<tr><td>[^abc]</td><td>�l�K�e�B�u�Z�b�g�B"a"�A"b"�A"c"�̂����ꂩ�łȂ��ꕶ���Ƀ}�b�`�B</td></tr>
<tr><td>[^a-z]</td><td>�l�K�e�B�u�Z�b�g�i�͈́j�B"a"����"z"�̂����ꂩ�łȂ��ꕶ���Ƀ}�b�`�B�l�K�e�B�u�Z�b�g�ƍ��݉\�B</td></tr>
<tr><td>a*</td><td>�A�X�^���X�N�B0�ȏ��a�Ƀ}�b�`�Ba�͉��炩�̐��K�\���B</td></tr>
<tr><td>a+</td><td>�v���X�B1�ȏ��a�Ƀ}�b�`�B</td></tr>
<tr><td>a?</td><td>�N�G�X�`�����B0��������1��a�Ƀ}�b�`�B</td></tr>
<tr><td>a{n,m}</td><td>n�ȏ�m�ȉ���a�Ƀ}�b�`�Bn�Am�͐��l�B</td></tr>
<tr><td>a{n,}</td><td>n�ȏ��a�Ƀ}�b�`�B</td></tr>
<tr><td>a{m}</td><td>m��a�Ƀ}�b�`�B</td></tr>
<tr><td>{<i>other-regular-expression</i>}</td><td>���̐��K�\���K��`other-regular-expression`�Ƀ}�b�`�B</td></tr>
<tr><td>"abc\\xyz\"pqr"</td><td>������abc\xyz"pqr�Ƀ}�b�`�B</td></tr>
<tr><td>\N</td><td>N��"a"�A'b"�A"f"�A"n"�A"r"�A"t"�A"v", "\\"�̎��A"\a"�A'\b"�A"\f"�A"\n"�A"\r"�A"\t"�A"\v"�A"\\"�ɕϊ�����}�b�`���܂��B<br/>N���p�^�[�����̉��Z�q�̏ꍇ�A���̕������̂��̂̌��ʂ𖳌��ɂ��A�}�b�`���܂��B</td></tr>
<tr><td>(a)</td><td>a�Ƀ}�b�`�B�D�揇�ʂ�1�ʂɕύX����܂��B</td></tr>
<tr><td>�i�D�揇��2�ʁj</td><td><hr/></td></tr>
<tr><td>ab</td><td>���K�\��a��b�̘A���Ƀ}�b�`�B</td></tr>
<tr><td>�i�D�揇��3�ʁj</td><td><hr/></td></tr>
<tr><td>a|b</td><td>���j�I���Ba��������b�Ƀ}�b�`�B</td></tr>
<tr><td>�i�D�揇��4�ʁj</td><td><hr/></td></tr>
<tr><td>a/b</td><td>�E�����B����b����������a�Ƀ}�b�`�i�Q�ƁF�E�����Œ�`�s�\�ȃp�^�[���j�B</td></tr>
<tr><td>^a</td><td>�n�b�g�Ba���s���i���s�̒���������̓}�b�`���O�Ώۂ̃V�[�P���X�̐擪�j�ɂ���ꍇ�}�b�`�B</td></tr>
<tr><td>a$</td><td>�G���h�I�u���C���Ba�̎����s�[�i���s�̒��O�������̓}�b�`���O�Ώۂ̃V�[�P���X�̏I�[�j�ɂ���ꍇ�Ƀ}�b�`�B</td></tr>
</table>

## �G�X�P�[�v�ɂ����钍�ӎ���
��\�I�Ȏ����̓v���O����Flex�ȂǂƈႢ�A���̃v���O�����ł͑S�Ẳ��Z�q�ɑ΂��Ă��̂��̂��}�b�`������ꍇ�A�ǂ̏ꏊ�ɂ��낤�Ƃ�"\"�ɂ��G�X�P�[�v���K�v�ƂȂ�܂��B

## �����N���X
���̃v���O�����ł́A�ȉ��̕����N���X���g�p�\�ł��B

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
���ꂼ��C�����isXXX�֐��ɑ������镶���N���X�Ƀ}�b�`���܂��B

## �o�̓t�@�C��
�o�͂��ꂽ�t�@�C���͈ȉ��̃w�b�_��include���܂��B  

    <utility>
    <iterator>
    <cstring>
    <cctype>
    <ctype.h>
cctype��ctype�̗�����include���Ă���̂�MSVC 10.0�ɂ�����`std::isblank`�����p�ł��Ȃ����߂ł��B  
�܂��͗�̓��͂����Ă݂܂��傤�B

    aaa
      whitespace   != " "*
      token        =  [a-zA-Z][a-zA-Z0-9]*
      value        =  ([1-9][0-9]*)|0
namespace aaa���Ɉȉ��̃R�[�h����������܂��B  

    namespace aaa{
        enum token{
            token_whitespace,
            token_token,
            token_value
        };

        class lexer{
        public:
            template<class InputIter>
            static std::pair<bool, InputIter> reg_whitespace(InputIter first, InputIter last){ /* �ȗ� */ }

            template<class InputIter>
            static std::pair<bool, InputIter> reg_token(InputIter first, InputIter last){ /* �ȗ� */ }

            template<class InputIter>
            static std::pair<bool, InputIter> reg_value(InputIter first, InputIter last){ /* �ȗ� */ }

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
`aaa::token`�́A���K�\���Ƀ}�b�`��������tokenizer�����o�֐�����insert\_iterator����ďo�͂����g�[�N����enum�ł��B  
`aaa::lexer::reg_whitespace`�A`aaa::lexer::reg_token`�A`aaa::lexer::reg_value`�͂��ꂼ����̓t�@�C���̐��K�\�����琶�����ꂽ�����o�֐��ł��B  
input\_iterator�̓��͂���}�b�`���邩�ǂ����𔻒肵�A�}�b�`�����ꍇ��first��true���Asecond�Ƀ}�b�`���������iterator��Ԃ��܂��B  
`aaa::lexer::tokenize`�����o�֐����Ŏg�p����Ă��܂����Apublic��static�ȃ����o�֐��Ȃ̂ŒP�̂Ŏg�p���鎖���\�ł��B  
`aaa::lexer::tokenize`�����o�֐���input\_iterator�̎n�[�ƏI�[����A���ݑ��݂��鐳�K�\���S�Ăɑ΂��ă}�b�`���邩�ǂ��������s���܂��B�g�[�N���𕪊��������ʂ́A��3������insert\_iterator��`std::make_pair(aaa::token, std::make_pair(first, last))`�Ƃ��Ēǉ�����܂��B�߂�l��`aaa::lexer::reg_XXX`�Ɠ����ŁA���炩�̐��K�\���Ƀ}�b�`�����ꍇfirst��true���A�����łȂ��ꍇ��false���������Asecond�ł͑����̏I�������ʒu��iterator���Ԃ���܂��B

# ���̑�
�o�͂��ꂽ�f�[�^�͂��̃f�[�^�̊�ƂȂ������̓f�[�^���쐬�����{�l�̂��̂ƂȂ�܂��B  
Copyright (c) 2012 uwanosora All Rights Reserved.
