#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <functional>
#include <string>
#include <utility>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <sstream>
#include <cstring>
#include <cctype>

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

    std::size_t lexical_cast(const std::string &value){
        std::size_t r;
        std::istringstream is(value);
        is >> r;
        return r;
    }

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
                << ind_0 << ind << "if(i >= " << n << " && i <= " << m << "){ match = true; }else{\n"
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
                    << ind_0 << ind << ind << "if(!match){\n"
                    << ind_0 << ind << ind << ind << "iter = iter_prime;\n"
                    << ind_0 << ind << ind << ind << "break;\n"
                    << ind_0 << ind << ind << "}\n";
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
            u->generate(os, ind_0);
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
            if(v){
                r = dynamic_cast<regexp_char_seq*>(u)->make_string();
                r += v->c;
            }else{
                r += u->c;
            }
            return std::move(r);
        }
    };

    class regexp_holder;

    class regexp_other_rule : public regexp{
    public:
        regexp_other_rule(regexp_holder *regexp_holder_ptr_) : regexp_holder_ptr(regexp_holder_ptr_){}

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
                if(p || q){
                    os << ind_0 << ind << ind << "(c == '" << tail->u->c << "')" << ext << "\n";
                }else if(r){
                    os << ind_0 << ind << ind << "((c >= '" << tail->u->u->c << "') && (c <= '" << tail->v->u->c << "'))" << ext << "\n";
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
                if(p || q){
                    os << ind_0 << ind << ind << "(c != '" << tail->u->c << "')" << ext << "\n";
                }else if(r){
                    os << ind_0 << ind << ind << "((c < '" << tail->u->u->c << "') || (c > '" << tail->v->u->c << "'))" << ext << "\n";
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
        reg_data(regexp_holder *regexp_holder_ptr_, const std::string &rule_name_) :
            regexp_holder_ptr(regexp_holder_ptr_),
            rule_name(rule_name_),
            ast(nullptr),
            ref_rule_name(rule_name)
        {}

        reg_data(const reg_data &other) :
            regexp_holder_ptr(other.regexp_holder_ptr),
            rule_name(other.rule_name),
            ast(other.ast->clone()),
            ref_rule_name(rule_name)
        {}

        reg_data(reg_data &&other) :
            regexp_holder_ptr(std::move(other.regexp_holder_ptr)),
            rule_name(std::move(other.rule_name)),
            ast(std::move(other.ast)),
            ref_rule_name(rule_name)
        {
            other.ast = nullptr;
        }
            
        ~reg_data(){
            delete ast;
        }

        void syntax_error(){
            throw(exception(rule_name + std::string(" - syntax error. (regular expression parser.)")));
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

        regexp *make_number(regexp *a){
            regexp_char *ptr = new regexp_char;
            ptr->u = a;
            ast = ptr;
            return ptr;
        }

        regexp *make_set(regexp *a, regexp *b, regexp *c, regexp *d){
            regexp *ptr;
            if(!b){
                ptr = new regexp_set;
            }else{
                ptr = new regexp_negative_set;
            }
            ptr->u = a;
            delete b;
            delete c;
            delete d;
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
            ast = ptr;
            return ptr;
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

        regexp *make_range(regexp *a, regexp *b){
            regexp_range *ptr = new regexp_range;
            ptr->u = a;
            ptr->v = b;
            ast = ptr;
            return ptr;
        }

        void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
            ast->generate(os, ind_0);
            //std::shared_ptr<const indent> ind = ind_0->clone(1);
            //std::shared_ptr<const indent>
            //    ind_1 = ind_0->nested_clone(),
            //    ind_2 = ind_1->nested_clone();
            //os
            //    << ind_0 << "template<class InputIter>\n"
            //    << ind_0 << "std::pair<bool, InputIter> " << rule_name << "(InputIter first, InputIter last){\n"
            //    << ind_0 << ind << "InputIter iter = first;\n"
            //    << ind_0 << ind << "bool match = true;\n";
            //ast->generate(os, ind_2);
            //os
            //    << ind_0 << ind << "return std::make_pair(match, iter);\n"
            //    << ind_0 << "}\n";
        }

    private:
        regexp_holder *regexp_holder_ptr;
        std::string rule_name;
        regexp *ast;

    public:
        const std::string &ref_rule_name;
    };

    class regexp_holder{
    public:
        regexp_holder(
            const std::string &file_name_,
            const std::string &namespace_a
        ) : file_name(file_name_), namespace_(namespace_a){}

        template<class InputIter>
        void add(std::string name, InputIter first, InputIter last){
            reg_data data(this, name);
            reg_parser::parser<regexp*, reg_data> parser(data);
            for(InputIter iter = first; iter != last; ++iter){
                parser.post(iter->first, iter->second);
            }
            parser.post(reg_parser::token_0, nullptr);
            regexp *ptr;
            if(!parser.accept(ptr)){
                std::cout << "parsing error\n";
            }
            reg_data_list.push_back(std::move(data));
            reg_data_map.insert(std::make_pair(name, &reg_data_list.back()));
        }

        const reg_data &get_other_reg_data(const std::string &str) const{
            return *reg_data_map.find(str)->second;
        }

        void generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0){
            std::shared_ptr<const indent> ind = ind_0->clone(1);
            std::shared_ptr<const indent>
                ind_1 = ind_0->nested_clone(),
                ind_2 = ind_1->nested_clone();
            std::string include_guard;
            for(std::size_t i = 0; i < file_name.size(); ++i){
                char c = file_name[i];
                if(std::isalnum(c)){
                    include_guard += static_cast<char>(std::toupper(c));
                }else{
                    include_guard += "_";
                }
            }
            include_guard += "_";
            os
                << "#ifndef " << include_guard << "\n"
                << "#define " << include_guard << "\n"
                << "\n"
                << "#include <utility>" << "\n"
                << "\n";
            if(namespace_.size() > 0){
                os
                    << "namespace " << namespace_ << "{" << "\n"
                    << "\n";
            }
            os
                << "enum token{" << "\n";
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
            os
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
                    << ind_0 << "static std::pair<bool, InputIter> " << iter->ref_rule_name << "(InputIter first, InputIter last){\n"
                    << ind_0 << ind << "InputIter iter = first;\n"
                    << ind_0 << ind << "bool match = true;\n";
                iter->generate(os, ind_1);
                os
                    << ind_0 << ind << "return std::make_pair(match, iter);\n"
                    << ind_0 << "}\n\n";
            }
            os
                << "};\n";
            if(namespace_.size() > 0){
                os
                    << "} // " << namespace_ << "\n"
                    << "\n";
            }
            os
                << "#endif // " << include_guard << "\n"
                << "\n";
        }

    private:
        std::string
            file_name,
            namespace_;
        std::list<reg_data> reg_data_list;
        std::map<std::string, reg_data*> reg_data_map;
    };

    void regexp_other_rule::generate(std::ostream &os, const std::shared_ptr<const indent> &ind_0) const{
        regexp_holder_ptr->get_other_reg_data(dynamic_cast<regexp_char_seq*>(u)->make_string());
    }
}

// !!
//#include "put_proto.hpp"

namespace paper_cutter{
    void test(){
        // !!
        //std::string str = "defghi";
        //std::string str = "aaabbbcghi";
        //std::cout << reg_test(str.begin(), str.end()).first << "\n";

        std::vector<std::pair<reg_parser::token, regexp_plain_char*>> token_vec;
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('a')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_star, new regexp_plain_char('*')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('b')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_plus, new regexp_plain_char('+')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('c')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_question, new regexp_plain_char('?')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_or, new regexp_plain_char('|')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_hat, new regexp_plain_char('^')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('d')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('e')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('f')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_slash, new regexp_plain_char('/')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('g')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('h')));
        token_vec.push_back(std::make_pair(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('i')));

        regexp_holder holder("put_proto.hpp", "test");
        holder.add("reg_test", token_vec.begin(), token_vec.end());
        std::shared_ptr<const indent> indent(new indent_space(1));
        std::ofstream ofile("put_proto.hpp");
        holder.generate(ofile, indent);

        //// !!
        //reg_data data("reg_test");
        //reg_parser::parser<regexp*, reg_data> parser(data);
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('a'));
        //parser.post(reg_parser::token_symbol_star, new regexp_plain_char('*'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('b'));
        //parser.post(reg_parser::token_symbol_plus, new regexp_plain_char('+'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('c'));
        //parser.post(reg_parser::token_symbol_question, new regexp_plain_char('?'));
        //parser.post(reg_parser::token_symbol_or, new regexp_plain_char('|'));
        //parser.post(reg_parser::token_symbol_hat, new regexp_plain_char('^'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('d'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('e'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('f'));
        //parser.post(reg_parser::token_symbol_slash, new regexp_plain_char('/'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('g'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('h'));
        //parser.post(reg_parser::token_symbol_any_non_metacharacter, new regexp_plain_char('i'));
        //parser.post(reg_parser::token_0, nullptr);
        //regexp *ptr;
        //if(!parser.accept(ptr)){
        //    std::cout << "parsing error\n";
        //}
        //std::ofstream ofile("put_proto.hpp");
        //data.generate(ofile);
        ////reg_data.generate(std::cout);
    }
}

int main(){
    paper_cutter::test();
    return 0;
}
