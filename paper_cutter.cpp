#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <fstream>
#include <functional>
#include <string>
#include <utility>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <locale>
#include <sstream>
#include <cstring>
#include <cctype>
#include <ctype.h>

namespace paper_cutter{
    class regexp;
}

#include "reg_parser.hpp"

namespace paper_cutter{
    class exception : public std::runtime_error{
    public:
        exception(const std::string &message) :
            std::runtime_error(message)
        {}
    };

    bool escape(char ch){
        switch(ch){
        case '\\':
        case 'a':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
        case 'v':
            return true;

        default:
            break;
        }
        return false;
    }

    bool isndigit(char ch){
        switch(ch){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            break;

        default:
            return false;
        }
        return true;
    }

    std::size_t lexical_cast(const std::string &value){
        for(std::size_t i = 0; i < value.size(); ++i){
            if(std::isdigit(value[i]) == 0){ throw(exception("'" + value + "' isn't digit.")); }
        }
        std::size_t r;
        std::istringstream is(value);
        is >> r;
        return r;
    }

    std::string lexical_cast(std::size_t value){
        std::string r;
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    class regexp_holder;
    class reg_data;

    class indent{
    public:
        virtual const char *str() const = 0;
        virtual std::shared_ptr<const indent> clone(std::size_t) const = 0;
        virtual std::shared_ptr<const indent> nested_clone() const = 0;
        virtual ~indent(){}
        std::size_t nest;
    };

    std::ostream &operator <<(std::ostream &os, const std::shared_ptr<const indent> &i){
        for(std::size_t count = 0; count < i->nest; ++count){
            os << i->str();
        }
        return os;
    }

    class indent_space : public indent{
    public:
        indent_space(std::size_t n){ nest = n; }

        virtual const char *str() const{
            return "    ";
        }

        virtual std::shared_ptr<const indent> clone(std::size_t n) const{
            indent_space *ptr = new indent_space(n);
            return std::shared_ptr<const indent>(ptr);
        }

        virtual std::shared_ptr<const indent> nested_clone() const{
            indent_space *ptr = new indent_space(nest + 1);
            return std::shared_ptr<const indent>(ptr);
        }
    };

    class indent_space8 : public indent{
    public:
        indent_space8(std::size_t n){ nest = n; }

        virtual const char *str() const{
            return "        ";
        }

        virtual std::shared_ptr<const indent> clone(std::size_t n) const{
            indent_space8 *ptr = new indent_space8(n);
            return std::shared_ptr<const indent>(ptr);
        }

        virtual std::shared_ptr<const indent> nested_clone() const{
            indent_space8 *ptr = new indent_space8(nest + 1);
            return std::shared_ptr<const indent>(ptr);
        }
    };

    class indent_tab : public indent{
    public:
        indent_tab(std::size_t n){ nest = n; }

        virtual const char *str() const{
            return "\t";
        }

        virtual std::shared_ptr<const indent> clone(std::size_t n) const{
            indent_tab *ptr = new indent_tab(n);
            return std::shared_ptr<const indent>(ptr);
        }

        virtual std::shared_ptr<const indent> nested_clone() const{
            indent_tab *ptr = new indent_tab(nest + 1);
            return std::shared_ptr<const indent>(ptr);
        }
    };

    class regexp{
    public:
        regexp() : c('\0'), u(nullptr), v(nullptr){}
        regexp(const regexp &other) : c(other.c), u(nullptr), v(nullptr){
            if(other.u){ u = u->clone(); }
            if(other.v){ v = v->clone(); }
        }

        regexp(regexp &&other) : c(std::move(other.c)), u(std::move(other.u)), v(std::move(other.v)){
            other.u = nullptr;
            other.v = nullptr;
        }

        virtual ~regexp(){
            delete u;
            delete v;
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const = 0;
        virtual regexp *clone() const = 0;

        template<class Drived>
        regexp *clone_impl() const{
            regexp *ptr = new Drived;
            ptr->c = c;
            if(u){ ptr->u = u->clone(); }
            if(v){ ptr->v = v->clone(); }
            return ptr;
        }

        char c;
        regexp *u, *v;
    };

    class regexp_reg : public regexp{
    public:
        virtual ~regexp_reg(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_reg>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            u->generate(os, ind_0);
        }
    };

    class regexp_union : public regexp{
    public:
        virtual ~regexp_union(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_union>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// union\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{ \n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n"
                << ind_0 << ind << "do{\n";
            const char
                *dec_match = "if(match){ break; }else{ iter = iter_prime; }\n",
                *dec_match_last = "if(!match){ iter = iter_prime; }\n";
            std::function<void(const regexp*)> recursive_put;
            recursive_put = [&](const regexp *ptr){
                if(dynamic_cast<const regexp_union*>(ptr)){
                    if(ptr->v){
                        recursive_put(ptr->u);
                        if(ptr->v){ os << ind_0 << ind << ind  << dec_match; }
                        ptr->v->generate(os, ind_2);
                        if(ptr->v == v){ os << ind_0 << ind << ind  << dec_match_last; }
                    }else{
                        ptr->u->generate(os, ind_2);
                        os
                            << ind_0 << ind << ind  << dec_match;
                    }
                }else{ ptr->generate(os, ind_2); }
            };
            recursive_put(this);
            os
                << ind_0 << ind << "}while(false);\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_complement : public regexp{
    public:
        virtual ~regexp_complement(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_complement>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// complement\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "InputIter iter_prime = iter, iter_prime_prime;\n";
            u->generate(os, ind_1);
            os
                << ind_0 << ind << "if(match){\n"
                << ind_0 << ind << ind << "iter_prime_prime = iter;\n"
                << ind_0 << ind << ind << "iter = iter_prime;\n";
            v->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "if(match){\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                << ind_0 << ind << ind << ind << "match = false;\n"
                << ind_0 << ind << ind << "}else{\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime_prime;\n"
                << ind_0 << ind << ind << ind << "match = true;\n"
                << ind_0 << ind << ind << "}\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_trailing_context : public regexp{
    public:
        virtual ~regexp_trailing_context(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_trailing_context>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// regexp_trailing_context\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n";
            u->generate(os, ind_1);
            os
                << ind_0 << ind << "if(match){\n";
            v->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "InputIter iter_prime_prime = iter;\n"
                << ind_0 << ind << ind << "if(match){ iter = iter_prime_prime; }else{ iter = iter_prime; }\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_n_to_m : public regexp{
    public:
        virtual ~regexp_n_to_m(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_n_to_m>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// n_to_m\n";
            os
                << ind_0 << "{\n"
                << ind_0 << ind << "InputIter iter_prime = iter, iter_prime_prime = iter;\n"
                << ind_0 << ind << "std::size_t i = 0;\n"
                << ind_0 << ind << "while(iter != last){\n";
            u->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "if(match){\n"
                << ind_0 << ind << ind << ind << "iter_prime = iter;\n"
                << ind_0 << ind << ind << ind << "++i;\n"
                << ind_0 << ind << ind << "}else{\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                << ind_0 << ind << ind << ind << "break;\n"
                << ind_0 << ind << ind << "}\n"
                << ind_0 << ind << "}\n"
                << ind_0 << ind << "if((i >= " << n << ") && (i <= " << m << ")){ match = true; }else{\n"
                << ind_0 << ind << ind << "match = false;\n"
                << ind_0 << ind << ind << "iter = iter_prime_prime;\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}\n";
        }

        std::size_t n, m;
    };

    class regexp_n : public regexp{
    public:
        virtual ~regexp_n(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_n>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// n\n";
            os
                << ind_0 << "{\n"
                << ind_0 << ind << "InputIter iter_prime = iter, iter_prime_prime = iter;\n"
                << ind_0 << ind << "std::size_t i = 0;\n"
                << ind_0 << ind << "while(iter != last){\n";
            u->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "if(match){\n"
                << ind_0 << ind << ind << ind << "iter_prime = iter;\n"
                << ind_0 << ind << ind << ind << "++i;\n"
                << ind_0 << ind << ind << "}else{\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                << ind_0 << ind << ind << ind << "break;\n"
                << ind_0 << ind << ind << "}\n"
                << ind_0 << ind << "}\n"
                << ind_0 << ind << "if(i >= " << n << "){ match = true; }else{\n"
                << ind_0 << ind << ind << "match = false;\n"
                << ind_0 << ind << ind << "iter = iter_prime_prime;\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}\n";
        }

        std::size_t n;
    };

    class regexp_m : public regexp{
    public:
        virtual ~regexp_m(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_m>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// m\n";
            os
                << ind_0 << "{\n"
                << ind_0 << ind << "InputIter iter_prime = iter, iter_prime_prime = iter;\n"
                << ind_0 << ind << "std::size_t i = 0;\n"
                << ind_0 << ind << "while(iter != last){\n";
            u->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "if(match){\n"
                << ind_0 << ind << ind << ind << "iter_prime = iter;\n"
                << ind_0 << ind << ind << ind << "++i;\n"
                << ind_0 << ind << ind << "}else{\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                << ind_0 << ind << ind << ind << "break;\n"
                << ind_0 << ind << ind << "}\n"
                << ind_0 << ind << "}\n"
                << ind_0 << ind << "if(i == " << m << "){ match = true; }else{\n"
                << ind_0 << ind << ind << "match = false;\n"
                << ind_0 << ind << ind << "iter = iter_prime_prime;\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}\n";
        }

        std::size_t m;
    };

    class regexp_concatenation : public regexp{
    public:
        virtual ~regexp_concatenation(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_concatenation>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// concatenation\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n"
                << ind_0 << ind << "do{\n";
            std::function<void(const regexp*)> recursive_put;
            recursive_put = [&](const regexp *ptr){
                if(ptr->v){
                    recursive_put(ptr->u);
                    ptr->v->generate(os, ind_2);
                }else{
                    ptr->generate(os, ind_2);
                }
                os
                    << ind_0 << ind << ind << "if(!match){ iter = iter_prime; break; }\n";
            };
            recursive_put(this);
            os
                << ind_0 << ind << "}while(false);\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_star : public regexp{
    public:
        virtual ~regexp_star(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_star>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// star\n";
            os
                << ind_0 << "{\n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n"
                << ind_0 << ind << "while(iter != last){\n";
            u->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "if(match){ iter_prime = iter; }else{\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                << ind_0 << ind << ind << ind << "match = true;\n"
                << ind_0 << ind << ind << ind << "break;\n"
                << ind_0 << ind << ind << "}\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_plus : public regexp{
    public:
        virtual ~regexp_plus(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_plus>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// plus\n";
            os
                << ind_0 << "do{\n";
            u->generate(os, ind_1);
            os
                << ind_0 << ind << "if(!match){ break; }\n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n"
                << ind_0 << ind << "while(iter != last){\n";
            u->generate(os, ind_2);
            os
                << ind_0 << ind << ind << "if(match){ iter_prime = iter; }else{\n"
                << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                << ind_0 << ind << ind << ind << "match = true;\n"
                << ind_0 << ind << ind << ind << "break;\n"
                << ind_0 << ind << ind << "}\n"
                << ind_0 << ind << "}\n"
                << ind_0 << "}while(false);\n";
        }
    };

    class regexp_question : public regexp{
    public:
        virtual ~regexp_question(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_question>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            u->generate(os, ind_0);
            //os << ind_0 << "// question\n";
            os
                << ind_0 << "match = true;\n";
        }
    };

    class regexp_group : public regexp{
    public:
        virtual ~regexp_group(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_group>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            u->generate(os, ind_0);
        }
    };

    class regexp_any : public regexp{
    public:
        virtual ~regexp_any(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_any>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            //os << ind_0 << "// any\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "++iter;\n"
                << ind_0 << ind << "match = true;\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_after_nline : public regexp{
    public:
        virtual ~regexp_after_nline(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_after_nline>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            //os << ind_0 << "// nline\n";
            os
                << ind_0 << "if(iter == first){ match = true; }else{\n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n"
                << ind_0 << ind << "--iter_prime;\n"
                << ind_0 << ind << "match = *iter_prime == '\\n';\n"
                << ind_0 << "}\n";
            u->generate(os, ind_0);
        }
    };

    class regexp_eos : public regexp{
    public:
        virtual ~regexp_eos(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_eos>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            //os << ind_0 << "// eos\n";
            os
                << ind_0 << "if(iter == last){ match = true; }else{ match = false; }\n";
        }
    };

    class regexp_plain_char : public regexp{
    public:
        regexp_plain_char(){}

        regexp_plain_char(char c_){ c = c_; }

        virtual ~regexp_plain_char(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_plain_char>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            //os << ind_0 << "// plain_char\n";
            os
                << ind_0 << "if(iter != last && *iter == '" << (c == '\\' ? "\\" : "") << c << "'){\n"
                << ind_0 << ind << "++iter;\n"
                << ind_0 << ind << "match = true;\n"
                << ind_0 << "}else{ match = false; }\n";
        }
    };

    class regexp_char : public regexp{
    public:
        virtual ~regexp_char(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_char>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            u->generate(os, ind_0);
        }
    };

    class regexp_meta_char : public regexp{
    public:
        virtual ~regexp_meta_char(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_meta_char>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            //os << ind_0 << "// meta_char\n";
            os
                << ind_0 << "if(iter != last && *iter == '" << (escape(c) ? "\\" : "") << c << "'){\n"
                << ind_0 << ind << "++iter;\n"
                << ind_0 << ind << "match = true;\n"
                << ind_0 << "}else{ match = false; }\n";
        }
    };

    class regexp_char_seq : public regexp{
    public:
        virtual ~regexp_char_seq(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_char_seq>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            assert(false);
        }

        std::string make_string() const{
            std::string r;
            if(u){
                if(u->c != '\0'){ r += u->c; }
                r += static_cast<regexp_char_seq*>(u)->make_string();
            }
            if(v){
                if(v->c != '\0'){ r += v->c; }
                r += static_cast<regexp_char_seq*>(v)->make_string();
            }
            return std::move(r);
        }
    };

    class regexp_str : public regexp_char_seq{
    public:
        virtual ~regexp_str(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_str>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            //os << ind_0 << "// str\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "InputIter iter_prime = iter;\n"
                << ind_0 << ind << "const char *str = \"" << make_string() << "\";\n"
                << ind_0 << ind << "std::size_t n = sizeof(\"" << make_string() << "\") / sizeof(char);\n"
                << ind_0 << ind << "std::size_t i = 0;\n"
                << ind_0 << ind << "while(str[i]){\n"
                << ind_0 << ind << ind << "if(str[i] != *iter){ break; }\n"
                << ind_0 << ind << ind << "++i, ++iter;\n"
                << ind_0 << ind << "}\n"
                << ind_0 << ind << "match = str[i] == '\\0';\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_other_rule : public regexp{
    public:
        regexp_other_rule(regexp_holder *regexp_holder_ptr_) :
            regexp_holder_ptr(regexp_holder_ptr_)
        {}

        virtual ~regexp_other_rule(){}

        virtual regexp *clone() const{
            regexp_other_rule *ptr = new regexp_other_rule(regexp_holder_ptr);
            ptr->c = c;
            ptr->u = u;
            ptr->v = v;
            return ptr;
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const;

    private:
        regexp_holder *regexp_holder_ptr;
    };

    class regexp_range : public regexp{
        virtual ~regexp_range(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_range>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            assert(false);
        }
    };

    class regexp_class : public regexp{
    public:
        virtual ~regexp_class(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_class>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::string str = dynamic_cast<regexp_char_seq*>(u)->make_string();
            if(
                str != "alnum" &&
                str != "alpha" &&
                str != "blank" &&
                str != "cntrl" &&
                str != "digit" &&
                str != "graph" &&
                str != "lower" &&
                str != "print" &&
                str != "punct" &&
                str != "space" &&
                str != "upper" &&
                str != "xdigit"
            ){ throw(exception("illegal class name.")); }
            if(str == "blank"){
                os
                    << ind_0 << "if(iter != last && isblank(*iter)){\n";
            }else{
                os
                    << ind_0 << "if(iter != last && std::is" << str << "(*iter)){\n";
            }
            os
                    << ind_0 << ind << "++iter;\n"
                    << ind_0 << ind << "match = true;\n"
                    << ind_0 << "}else{ match = false; }\n";
        }
    };

    class regexp_set : public regexp{
    public:
        virtual ~regexp_set(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_set>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            //os << ind_0 << "// set\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "char c = *iter;\n"
                << ind_0 << ind << "if(\n";
            auto put = [&](regexp *ptr, const char *ext){
                regexp *tail = ptr->u;
                regexp_char *p = dynamic_cast<regexp_char*>(tail);
                regexp_meta_char *q = p == nullptr ? dynamic_cast<regexp_meta_char*>(tail) : nullptr;
                regexp_range *r = p == nullptr && q == nullptr ? dynamic_cast<regexp_range*>(tail) : nullptr;
                regexp_class *c = r == nullptr && p == nullptr && q == nullptr ? dynamic_cast<regexp_class*>(tail) : nullptr;
                if(p || q){
                    char ch = tail->u->c;
                    os << ind_0 << ind << ind << "(c == '" << (q || escape(tail->u->c) ? "\\" : "") << ch << "')" << ext << "\n";
                }else if(r){
                    char ch_a = tail->u->u->c, ch_b = tail->v->u->c;
                    const char *escape_a = dynamic_cast<regexp_meta_char*>(tail->u->u) && escape(tail->u->u->c) ? "\\" : "";
                    const char *escape_b = dynamic_cast<regexp_meta_char*>(tail->v->u) && escape(tail->v->u->c) ? "\\" : "";
                    os << ind_0 << ind << ind << "((c >= '" << escape_b << ch_a << "') && (c <= '" << escape_b << ch_b << "'))" << ext << "\n";
                }else if(c){
                    os << ind_0 << ind << ind << "(std::is" << dynamic_cast<regexp_char_seq*>(c->u)->make_string() << "(c))" << ext << "\n";
                }else{ assert(false); }
            };
            std::function<void(regexp*)> recursive_put;
            recursive_put = [&](regexp *ptr){
                if(ptr->v){
                    recursive_put(ptr->u);
                    put(ptr->v, ptr != u ? " ||" : "");
                }else{
                    put(ptr->u, ptr != u ? " ||" : "");
                }
            };
            recursive_put(u);
            os
                << ind_0 << ind << "){\n"
                << ind_0 << ind << ind << "++iter;\n"
                << ind_0 << ind << ind << "match = true;\n"
                << ind_0 << ind << "}else{ match = false; }\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_negative_set : public regexp{
    public:
        virtual ~regexp_negative_set(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_negative_set>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            //os << ind_0 << "// negative_set\n";
            os
                << ind_0 << "if(iter == last){ match = false; }else{\n"
                << ind_0 << ind << "char c = *iter;\n"
                << ind_0 << ind << "if(\n";
            auto put = [&](regexp *ptr, const char *ext){
                regexp *tail = ptr->u;
                regexp_char *p = dynamic_cast<regexp_char*>(tail);
                regexp_meta_char *q = p == nullptr ? dynamic_cast<regexp_meta_char*>(tail) : nullptr;
                regexp_range *r = p == nullptr && q == nullptr ? dynamic_cast<regexp_range*>(tail) : nullptr;
                regexp_class *c = r == nullptr && p == nullptr && q == nullptr ? dynamic_cast<regexp_class*>(tail) : nullptr;
                if(p || q){
                    char ch = tail->u->c;
                    os << ind_0 << ind << ind << "(c != '" << (q || escape(tail->u->c) ? "\\" : "") << ch << "')" << ext << "\n";
                }else if(r){
                    char ch_a = tail->u->u->c, ch_b = tail->v->u->c;
                    const char *escape_a = dynamic_cast<regexp_meta_char*>(tail->u->u) && escape(tail->u->u->c) ? "\\" : "";
                    const char *escape_b = dynamic_cast<regexp_meta_char*>(tail->v->u) && escape(tail->v->u->c) ? "\\" : "";
                    os << ind_0 << ind << ind << "((c < '" << escape_a << ch_a << "') || (c > '" << escape_b << ch_b << "'))" << ext << "\n";
                }else if(c){
                    os << ind_0 << ind << ind << "(std::is" << dynamic_cast<regexp_char_seq*>(c->u)->make_string() << "(c) == 0)" << ext << "\n";
                }else{ assert(false); }
            };
            std::function<void(regexp*)> recursive_put;
            recursive_put = [&](regexp *ptr){
                if(ptr->v){
                    recursive_put(ptr->u);
                    put(ptr->v, ptr != u ? " &&" : "");
                }else{
                    put(ptr->u, ptr != u ? " &&" : "");
                }
            };
            recursive_put(u);
            os
                << ind_0 << ind << "){\n"
                << ind_0 << ind << ind << "++iter;\n"
                << ind_0 << ind << ind << "match = true;\n"
                << ind_0 << ind << "}else{ match = false; }\n"
                << ind_0 << "}\n";
        }
    };

    class regexp_set_items : public regexp{
    public:
        virtual ~regexp_set_items(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_set_items>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            assert(false);
        }
    };

    class regexp_set_item : public regexp{
    public:
        virtual ~regexp_set_item(){}

        virtual regexp *clone() const{
            return clone_impl<regexp_set_item>();
        }

        virtual void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            assert(false);
        }
    };

    class reg_data{
    public:
        reg_data(regexp_holder *regexp_holder_ptr_, const std::string &rule_name_, std::size_t line_num_, bool dispose_) :
            regexp_holder_ptr(regexp_holder_ptr_),
            rule_name(rule_name_),
            line_num(line_num_),
            dispose(dispose_),
            ast(nullptr),
            ref_rule_name(rule_name),
            ref_line_num(line_num)
        {}

        reg_data(const reg_data &other) :
            regexp_holder_ptr(other.regexp_holder_ptr),
            rule_name(other.rule_name),
            line_num(other.line_num),
            dispose(other.dispose),
            ast(other.ast->clone()),
            ref_rule_name(rule_name),
            ref_line_num(line_num)
        {}

        reg_data(reg_data &&other) :
            regexp_holder_ptr(std::move(other.regexp_holder_ptr)),
            rule_name(std::move(other.rule_name)),
            line_num(std::move(other.line_num)),
            dispose(std::move(other.dispose)),
            ast(std::move(other.ast)),
            ref_rule_name(rule_name),
            ref_line_num(line_num)
        { other.ast = nullptr; }
            
        ~reg_data(){ delete ast; }

        void syntax_error(){
            throw(exception("syntax error."));
        }

        void stack_overflow(){}

        void upcast(regexp *&x, regexp *y){
            x = y;
        }

        void downcast(regexp *&x, regexp *y){
            x = y;
        }

        void downcast(char &x, regexp *y){
            if(y){
                x = y->c;
                delete y;
            }
        }

        regexp *make_regexp(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_lv0_regexp(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_lv1_regexp(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_lv2_regexp(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_lv3_regexp(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_union(regexp *a, regexp *b, regexp *d_0){
            delete d_0;
            regexp_union *ptr = new regexp_union;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        regexp *make_complement(regexp *a, regexp *b, regexp *d_0){
            delete d_0;
            regexp_complement *ptr = new regexp_complement;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        regexp *make_trailing_context(regexp *a, regexp *b, regexp *d_0){
            delete d_0;
            regexp_trailing_context *ptr = new regexp_trailing_context;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        regexp *make_n_to_m(regexp *a, regexp *b, regexp *c, regexp *d, regexp *d_0, regexp *d_1){
            delete d_0;
            delete d_1;
            std::function<void(regexp*, std::string&)> to_num;
            to_num = [&](regexp *ptr, std::string &str){
                if(ptr->v){
                    to_num(ptr->u, str);
                    str += ptr->v->u->c;
                }else{ str += ptr->u->c; }
            };
            regexp *r = nullptr;
            if(c && d){
                std::string str;
                regexp_n_to_m *ptr = new regexp_n_to_m;
                ptr->u = a;
                to_num(b, str);
                ptr->n = lexical_cast(str);
                str.clear();
                to_num(c, str);
                ptr->m = lexical_cast(str);
                r = ptr;
            }else if(!c && d){
                std::string str;
                regexp_n *ptr = new regexp_n;
                ptr->u = a;
                to_num(b, str);
                ptr->n = lexical_cast(str);
                r = ptr;
            }else if(!c && !d){
                std::string str;
                regexp_m *ptr = new regexp_m;
                ptr->u = a;
                to_num(b, str);
                ptr->m = lexical_cast(str);
                r = ptr;
            }
            if(!r){ throw(exception("illegal form to 'r{n, m}'.")); }
            return r;
        }

        regexp *make_comma(regexp *a){
            return a;
        }

        regexp *make_char_seq_opt(regexp *a){
            return a;
        }

        regexp *make_char_seq(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_char_seq(regexp *a, regexp *b){
            regexp_char_seq *ptr = new regexp_char_seq;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        regexp *make_concatenation(regexp *a, regexp *b){
            regexp_concatenation *ptr = new regexp_concatenation;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        regexp *make_star(regexp *a, regexp *b){
            delete b;
            regexp_star *ptr = new regexp_star;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_plus(regexp *a, regexp *b){
            delete b;
            regexp_plus *ptr = new regexp_plus;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_question(regexp *a, regexp *b){
            delete b;
            regexp_question *ptr = new regexp_question;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_elementary_regexp(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_group(regexp *a, regexp *b, regexp *c){
            delete b;
            delete c;
            regexp_group *ptr = new regexp_group;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_str(regexp *a, regexp *b, regexp *c){
            delete b;
            delete c;
            regexp_str *ptr = new regexp_str;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_other_rule(regexp *a, regexp *b, regexp *c){
            delete b;
            delete c;
            regexp_other_rule *ptr = new regexp_other_rule(regexp_holder_ptr);
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_any(regexp *a){
            delete a;
            regexp_any *ptr = new regexp_any;
            ast = ptr;
            return ptr;
        }

        regexp *make_after_nline(regexp *a, regexp *b){
            delete b;
            regexp_after_nline *ptr = new regexp_after_nline;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_eos(regexp *a){
            delete a;
            regexp_eos *ptr = new regexp_eos;
            ast = ptr;
            return ptr;
        }

        regexp *make_char(regexp *a){
            regexp_char *ptr = new regexp_char;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_char(regexp *a, regexp *b){
            delete b;
            regexp_char *ptr = new regexp_char;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_escape_char(regexp *a){
            return a;
        }

        regexp *make_number(regexp *a){
            regexp_char *ptr = new regexp_char;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_hat(regexp *a){
            ast = a;
            return a;
        }

        regexp *make_meta_char(regexp *a){
            regexp_meta_char *ptr = new regexp_meta_char;
            ptr->c = a->c;
            delete a;
            ast = ptr;
            return ptr;
        }

        regexp *make_meta_char(regexp *a, regexp *b){
            delete b;
            return make_meta_char(a);
        }

        regexp *make_set_or_class(regexp *a, regexp *b, regexp *c){
            delete b;
            delete c;
            return a;
        }

        regexp *make_class_content(regexp *a, regexp *b, regexp *c){
            delete b;
            delete c;
            regexp *ptr = new regexp_class;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_set_content(regexp *a, regexp *b){
            regexp *ptr;
            if(!b){
                ptr = new regexp_set;
            }else{
                ptr = new regexp_negative_set;
            }
            ptr->u = a;
            delete b;
            ast = ptr;
            return ptr;
        }

        regexp *make_set_or_class_content(regexp *a){
            return a;
        }

        regexp *make_set_items(regexp *a){
            regexp_set_items *ptr = new regexp_set_items;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_set_items(regexp *a, regexp *b){
            regexp_set_items *ptr = new regexp_set_items;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        regexp *make_set_item(regexp *a){
            regexp_set_item *ptr = new regexp_set_item;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_range(regexp *a, regexp *b, regexp *c){
            delete c;
            regexp_range *ptr = new regexp_range;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            ast->generate(os, ind_0);
        }

        bool dispose_flag() const{
            return dispose;
        }

    private:
        regexp_holder *regexp_holder_ptr;
        std::string rule_name;
        std::size_t line_num;
        bool dispose;
        regexp *ast;

    public:
        const std::string &ref_rule_name;
        const std::size_t &ref_line_num;
    };

    class regexp_holder{
    public:
        regexp_holder(
            const std::string &file_name_,
            const std::string &namespace_a
        ) : file_name(file_name_), namespace_(namespace_a){}

        template<class InputIter>
        void add(const std::string &name, std::size_t line_num, bool dispose_flag, InputIter first, InputIter last){
            if(reg_data_map.find(name) != reg_data_map.end()){
                throw(exception(lexical_cast(line_num) + ":'" + name + "' was detected."));
            }
            reg_data data(this, name, line_num, dispose_flag);
            reg_parser::parser<regexp*, reg_data> parser(data);
            try{
                for(InputIter iter = first; iter != last; ++iter){
                    parser.post(iter->first, iter->second);
                }
                parser.post(reg_parser::token_0, nullptr);
                regexp *ptr;
                if(!parser.accept(ptr)){
                    throw(exception("parsing error."));
                }
            }catch(std::runtime_error e){
                throw(exception(lexical_cast(line_num) + ":" + e.what()));
            }
            reg_data_list.push_back(std::move(data));
            reg_data_map.insert(std::make_pair(name, &reg_data_list.back()));
        }

        const reg_data &get_other_reg_data(const std::string &str) const{
            std::map<std::string, reg_data*>::const_iterator find_result = reg_data_map.find(str);
            if(find_result == reg_data_map.end()){
                throw(exception("exp '" + str + "' is not found."));
            }
            return *find_result->second;
        }

        void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0, bool kp19pp = false) const{
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            std::string include_guard;
            if(!kp19pp){
                for(std::size_t i = 0; i < file_name.size(); ++i){
                    char c = file_name[i];
                    if(std::isalnum(c)){
                        include_guard += static_cast<char>(std::toupper(c));
                    } else{
                        include_guard += "_";
                    }
                }
                include_guard += "_";
            }
            if(!kp19pp){
                os
                    << "#ifndef " << include_guard << "\n"
                    << "#define " << include_guard << "\n"
                    << "\n";
            }
            os
                << "#include <utility>" << "\n"
                << "#include <iterator>" << "\n"
                << "#include <cstring>" << "\n"
                << "#include <cctype>" << "\n"
                << "#include <ctype.h>" << "\n"
                << "\n";
            if(namespace_.size() > 0){
                os
                    << "namespace " << namespace_ << "{" << "\n"
                    << "\n";
            }
            os
                << "enum token{" << "\n"
                << ind_0 << "token_0,\n";
            for(
                std::list<reg_data>::const_iterator iter = reg_data_list.begin(), end = reg_data_list.end(), dummy;
                iter != end;
                ++iter
            ){
                dummy = iter;
                ++dummy;
                os
                    << ind << "token_" << iter->ref_rule_name << (dummy == end ? "" : ",") << "\n";
            }
            os << "};\n";

            os
                << "template<class Iter>" << "\n"
                << "struct iterator{" << "\n"
                << ind << "iterator() = default;" << "\n"
                << ind << "iterator(const iterator &other) :" << "\n"
                << ind << ind << "end(other.end)," << "\n"
                << ind << ind << "char_count(other.char_count), line_count(other.line_count)," << "\n"
                << ind << ind << "value(other.value)" << "\n"
                << ind << "{}" << "\n"
                << ind << "iterator(const Iter &iter) :" << "\n"
                << ind << ind << "end(iter)," << "\n"
                << ind << ind << "char_count(0), line_count(0)," << "\n"
                << ind << ind << "value(token())" << "\n"
                << ind << "{}" << "\n"
                << ind << "~iterator() = default;" << "\n"
                << ind << "iterator &operator =(const Iter &other){" << "\n"
                << ind << ind << "end = other;" << "\n"
                << ind << ind << "++end;" << "\n"
                << ind << ind << "return *this;" << "\n"
                << ind << "}" << "\n"
                << ind << "iterator &operator =(const iterator &other){" << "\n"
                << ind << ind << "end = other.end;" << "\n"
                << ind << ind << "char_count = other.char_count, line_count = other.line_count;" << "\n"
                << ind << ind << "value = other.value;" << "\n"
                << ind << ind << "return *this;" << "\n"
                << ind << "}" << "\n"
                << ind << "iterator &operator ++(){" << "\n"
                << ind << ind << "if(*end == '\\n'){" << "\n"
                << ind << ind << ind << "char_count = 0;" << "\n"
                << ind << ind << ind << "++line_count;" << "\n"
                << ind << ind << "} else{" << "\n"
                << ind << ind << ind << "++char_count;" << "\n"
                << ind << ind << "}" << "\n"
                << ind << ind << "++end;" << "\n"
                << ind << ind << "return *this;" << "\n"
                << ind << "}" << "\n"
                << ind << "bool operator ==(const Iter &other) const{" << "\n"
                << ind << ind << "Iter last = end;" << "\n"
                << ind << ind << "--last;" << "\n"
                << ind << ind << "return last == other;" << "\n"
                << ind << "}" << "\n"
                << ind << "bool operator ==(const iterator &other) const{" << "\n"
                << ind << ind << "return end == other.end;" << "\n"
                << ind << "}" << "\n"
                << ind << "template<class Other>" << "\n"
                << ind << "bool operator !=(const Other &other) const{" << "\n"
                << ind << ind << "return !(*this == other);" << "\n"
                << ind << "}" << "\n"
                << ind << "Iter end;" << "\n"
                << ind << "std::size_t char_count, line_count;" << "\n"
                << ind << "token value;" << "\n"
                << "};" << "\n"
                << "\n"
                << "class lexer{" << "\n"
                << "public:" << "\n";
            for(
                std::list<reg_data>::const_iterator iter = reg_data_list.begin(), end = reg_data_list.end(), dummy;
                iter != end;
                ++iter
            ){
                os
                    << ind_0 << "template<class InputIter>\n"
                    << ind_0 << "static std::pair<bool, iterator<InputIter>> reg_" << iter->ref_rule_name << "(InputIter first, InputIter last){\n"
                    << ind_0 << ind << "InputIter iter = first;\n"
                    << ind_0 << ind << "bool match = true;\n";
                recursive_check_cache.clear();
                insert_recursive_cache(iter->ref_rule_name);
                try{
                    iter->generate(os, ind_1);
                }catch(std::runtime_error e){
                    throw(exception(lexical_cast(iter->ref_line_num) + ":" + e.what()));
                }
                os
                    << ind_0 << ind << "return std::make_pair(match, iter);\n"
                    << ind_0 << "}\n\n";
            }
            os
                << ind_0 << "template<class InputIter>\n"
                << ind_0 << "static std::pair<token, InputIter> apply(InputIter first, InputIter last){\n"
                << ind_0 << ind << "InputIter iter = first;\n"
                << ind_0 << ind << "std::pair<bool, iterator<InputIter>> result;\n";
            for(
                std::list<reg_data>::const_iterator iter = reg_data_list.begin(), end = reg_data_list.end(), dummy;
                iter != end;
                ++iter
            ){
                os
                    << ind_0 << ind << "result = reg_" << iter->ref_rule_name << "(iter, last);" << "\n"
                    << ind_0 << ind << "if(result.first){" << "\n"
                    << ind_0 << ind << ind << "iter = result.second.end;" << "\n"
                    << ind_0 << ind << ind << "return std::make_pair(token_" << iter->ref_rule_name << ", iter);\n"
                    << ind_0 << ind << "}\n";
            }
            os
                << ind_0 << ind << "return std::make_pair(token_0, iter);\n"
                << ind_0 << "}\n"
                << "\n";
            os
                << ind_0 << "template<class InputIter, class InsertFunctor>" << "\n"
                << ind_0 << "static std::pair<bool, iterator<InputIter>> tokenize(InputIter first, InputIter last, const InsertFunctor &f){" << "\n"
                << ind_0 << ind << "InputIter iter = first;" << "\n"
                << ind_0 << ind << "std::pair<bool, iterator<InputIter>> result;" << "\n"
                << ind_0 << ind << "while(iter != last){"<< "\n";
            for(
                std::list<reg_data>::const_iterator iter = reg_data_list.begin(), end = reg_data_list.end(), dummy;
                iter != end;
                ++iter
            ){
                os
                    << ind_0 << ind << ind << "result = reg_"<< iter->ref_rule_name << "(iter, last);" << "\n"
                    << ind_0 << ind << ind << "if(result.first){" << "\n";
                if(!iter->dispose_flag()){
                    os
                        << ind_0 << ind << ind << ind << "f(token_" << iter->ref_rule_name << ", iter, result.second);" << "\n";
                }
                os
                    << ind_0 << ind << ind << ind << "iter = result.second;" << "\n"
                    << ind_0 << ind << ind << ind << "continue;\n"
                    << ind_0 << ind << ind << "}\n";
            }
            os
                << ind_0 << ind << ind << "break;" << "\n"
                << ind_0 << ind << "}" << "\n"
                << ind_0 << ind << "return result;" << "\n"
                << ind_0 << "}" << "\n";
            os
                << "};\n";
            if(namespace_.size() > 0){
                os
                    << "} // namespace " << namespace_ << "\n"
                    << "\n";
            }
            if(!kp19pp){
                os
                    << "#endif // " << include_guard << "\n"
                    << "\n";
            }
        }

        void insert_recursive_cache(const std::string &other_name) const{
            if(recursive_check_cache.find(other_name) != recursive_check_cache.end()){
                throw(exception("recursive was detected."));
            }
            recursive_check_cache.insert(other_name);
        }

    private:
        std::string
            file_name,
            namespace_;
        std::list<reg_data> reg_data_list;
        std::map<std::string, reg_data*> reg_data_map;
        mutable std::set<std::string> recursive_check_cache;
    };

    void regexp_other_rule::generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
        regexp_char_seq *cast_result = dynamic_cast<regexp_char_seq*>(u);
        std::string other_name;
        if(cast_result){
            other_name = cast_result->make_string();
        }else{
            other_name += u->u->c;
        }
        regexp_holder_ptr->insert_recursive_cache(other_name);
        regexp_holder_ptr->get_other_reg_data(other_name).generate(os, ind_0);
    }
}

#include "parser.hpp"

namespace paper_cutter{
    struct plain_reg_data{
        plain_reg_data() : name(), reg(){}
        plain_reg_data(const plain_reg_data &other) : name(other.name), reg(other.reg){}
        plain_reg_data(plain_reg_data &&other) : name(std::move(other.name)), reg(std::move(other.reg)){}

        std::string name, reg;
        bool dispose;
    };

    class lexical_data{
    public:
        void syntax_error() const{
            throw(exception("lexical error."));
        }

        void stack_overflow() const{}

        void upcast(std::string &x, const std::string &y) const{
            x = y;
        }

        void downcast(std::string &x, const std::string &y) const{
            x = y;
        }

        void downcast(std::string &x, const char y) const{
            x = y;
        }

        void downcast(char &x, const std::string &y) const{
            x = y[0];
        }

        std::string make_line(std::string name, std::string ex_opt, std::string reg){
            data.name = name;
            data.reg = reg;
            data.dispose = !ex_opt.empty();
            return "";
        }

        std::string make_dispose_rule(){
            return "dispose";
        }

        std::string make_reg_name(char c){
            std::string str;
            str += c;
            return std::move(str);
        }

        std::string make_reg_name(std::string str, char c){
            str += c;
            return std::move(str);
        }

        std::string make_reg_sequence(std::string str){
            return std::move(str);
        }

        std::string make_reg_sequence(std::string str, std::string rts){
            str += rts;
            return std::move(str);
        }

        std::string make_reg_char(char c){
            std::string str;
            str += c;
            return std::move(str);
        }

        plain_reg_data data;
    };

    namespace{
        std::string str_to_upper(const char *str){
            std::locale l;
            std::string ret;
            for(std::size_t i = 0; str[i]; ++i){
                ret += std::toupper(str[i], l);
            }
            return ret;
        }
    }

    class commandline_options{
    public:
        enum language_enum{
            language_cpp
            //language_csharp,
            //language_d,
            //language_java,
            //language_javascript
        };

        enum indent_enum{
            indent_space,
            indent_space4,
            indent_space8,
            indent_tab
        };

        commandline_options() :
            ifile_path_(), ofile_path_(), language_(language_cpp), indent_(indent_space4)
        {}

        bool get(int argc, char **argv){
            int state = 0;
            for(int index = 1; index < argc; ++index){
                if(argv[index][0] == '-'){
                    std::string str = str_to_upper(argv[index]);
                    if(
                        str == "-C++" ||
                        str == "-CPP"
                    ){
                        language_ = language_cpp;
                        continue;
                    }
                    if(str == "-INDENT=SPACE"){
                        indent_ = indent_space;
                        continue;
                    }
                    if(str == "-INDENT=SPACE4"){
                        indent_ = indent_space4;
                        continue;
                    }
                    if(str == "-INDENT=SPACE8"){
                        indent_ = indent_space8;
                        continue;
                    }
                    if(str == "-INDENT=TAB"){
                        indent_ = indent_tab;
                        continue;
                    }
                    std::cerr << "unknown options '" << argv[index] << "'.\n";
                    return false;
                }
                switch(state){
                case 0: ifile_path_ = argv[index]; ++state; break;
                case 1: ofile_path_ = argv[index]; ++state; break;
                default:
                    std::cerr << "too many arguments.\n";
                    return false;
                }
            }
            if(state < 2){
                std::cout << "paper_cutter usage: paper_cutter [ -c++ | -indent=space | -indent=space8 | -indent=tab ] ifile_name ofile_name\n";
                return false;
            }
            return true;
        }
        const std::string &ifile_path() const{
            return ifile_path_;
        }

        std::string ifile_name() const{
            return file_name(ifile_path_);
        }

        const std::string &ofile_path() const{
            return ofile_path_;
        }

        std::string ofile_name() const{
            return file_name(ofile_path_);
        }

        language_enum language() const{
            return language_;
        }

        indent_enum indent() const{
            return indent_;
        }

    private:
        std::string file_name(const std::string &str) const{
            std::string ret;
            for(std::size_t i = 0, length = str.size(); i < length; ++i){
                char c = str[length - i - 1];
                if(c == '/' || c == '\\'){ break; }
                ret += c;
            }
            std::reverse(ret.begin(), ret.end());
            return ret;
        }

        std::string ifile_path_, ofile_path_;
        language_enum language_;
        indent_enum indent_;
    };

    int main(int argc, char **argv){
        paper_cutter::commandline_options co;
        if(!co.get(argc, argv)){
            return -1;
        }
        std::ifstream ifile(co.ifile_path().c_str());
        if(!ifile){
            std::cerr << "can't open ifile '" << co.ifile_path() << "'.\n";
            return -1;
        }
        std::ofstream ofile(co.ofile_path().c_str());
        if(!ofile){
            std::cerr << "can't open ofile '" << co.ofile_path() << "'.\n";
            return -1;
        }
        std::size_t line_num = 1;
        try{
            std::string line, namespace_;
            if(!std::getline(ifile, line)){ throw(exception(lexical_cast(line_num) + ":lexical error.")); }
            ++line_num;
            {
                std::stringstream ss(line);
                ss >> namespace_;
            }
            regexp_holder holder(co.ofile_name(), namespace_);
            for(; std::getline(ifile, line); ++line_num){
                if(
                    line.empty() ||
                    [&]() -> bool{
                        for(std::size_t i = 0; i < line.size(); ++i){
                            if(std::isspace(line[i]) == 0){ return false; }
                        }
                        return true;
                    }()
                ){ continue; }
                lexical_data ld;
                parser::parser<std::string, lexical_data> plexer(ld);
                try{
                    for(std::size_t i = 0; i < line.size(); ++i){
                        std::string c;
                        c += line[i];
                        bool r = false;
                        switch(c[0]){
                        case '|': case '*': case '+':
                        case '?': case '(': case ')':
                        case '{': case '}': case '.':
                        case '$': case '\\': case '[':
                        case '^': case ']': case '-':
                        case ',': case ':': case '"':
                            r = plexer.post(parser::token_symbol_any_meta_char, c);
                            break;

                        case '=':
                            r = plexer.post(parser::token_symbol_equal, c);
                            break;

                        case '!':
                            r = plexer.post(parser::token_symbol_exclamation, c);
                            break;

                        default:
                            if(std::isspace(c[0])){
                                r = plexer.post(parser::token_symbol_space, c);
                            }else if(isndigit(c[0])){
                                r = plexer.post(parser::token_symbol_number, c);
                            }else{
                                r = plexer.post(parser::token_symbol_any_char, c);
                            }
                        }
                        if(r){ throw(exception(lexical_cast(line_num) + ":lexical erorr.")); }
                    }
                }catch(std::runtime_error e){
                    throw(exception(lexical_cast(line_num) + ":" + e.what()));
                }
                {
                    std::string str;
                    plexer.post(parser::token_0, str);
                }
                std::vector<std::pair<reg_parser::token, regexp_plain_char*>> token_vec;
                for(std::size_t i = 0; i < ld.data.reg.size(); ++i){
                    char c = ld.data.reg[i];
                    switch(c){
                    case '|':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_or, new regexp_plain_char('|')));
                        break;

                    case '*':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_star, new regexp_plain_char('*')));
                        break;

                    case '+':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_plus, new regexp_plain_char('+')));
                        break;

                    case '?':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_question, new regexp_plain_char('?')));
                        break;

                    case '(':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_left_pare, new regexp_plain_char('(')));
                        break;

                    case ')':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_right_pare, new regexp_plain_char(')')));
                        break;

                    case '{':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_left_brace, new regexp_plain_char('{')));
                        break;

                    case '}':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_right_brace, new regexp_plain_char('}')));
                        break;

                    case '.':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_dot, new regexp_plain_char('.')));
                        break;

                    case '$':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_eos, new regexp_plain_char('$')));
                        break;

                    case '\\':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_backslash, new regexp_plain_char('\\')));
                        break;

                    case '[':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_set_left_bracket, new regexp_plain_char('[')));
                        break;

                    case '^':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_hat, new regexp_plain_char('^')));
                        break;

                    case ']':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_set_right_bracket, new regexp_plain_char(']')));
                        break;

                    case '-':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_minus, new regexp_plain_char('-')));
                        break;

                    case ',':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_comma, new regexp_plain_char(',')));
                        break;

                    case ':':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_colon, new regexp_plain_char(':')));
                        break;

                    case '"':
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_double_quote, new regexp_plain_char('"')));
                        break;

                    default:
                        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char(c)));;
                    }
                }
                holder.add(ld.data.name, line_num, ld.data.dispose, token_vec.begin(), token_vec.end());
            }
            std::shared_ptr<const indent> indent;
            switch(co.indent()){
            case commandline_options::indent_space:
            case commandline_options::indent_space4:
                indent.reset(new indent_space(1));
                break;

            case commandline_options::indent_space8:
                indent.reset(new indent_space8(1));
                break;

            case commandline_options::indent_tab:
                indent.reset(new indent_tab(1));
            }
            holder.generate(ofile, indent);
        }catch(std::runtime_error e){
            std::cerr << e.what() << "\n";
            return -1;
        }
        return 0;
    }
}

int main(int argc, char **argv){
    int argc_ = 5;
    char *argv_[] = { "dummy", "-c++", "-indent=space", "ifile.txt", "ofile.hpp" };
    return paper_cutter::main(argc, argv);
}
