#ifndef OFILE_HPP_
#define OFILE_HPP_

#include <utility>
#include <iterator>
#include <cstring>
#include <cctype>

namespace aaa{

enum token{
    token_foo,
    token_bar,
    token_hoge
};

class lexer{
public:
    template<class InputIter>
    static std::pair<bool, InputIter> reg_foo(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{ 
            InputIter iter_prime = iter;
            do{
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    do{
                        if(iter != last && *iter == '+'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){ iter = iter_prime; break; }
                    }while(false);
                }
                if(match){ break; }else{ iter = iter_prime; }
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    do{
                        if(iter != last && *iter == '-'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){ iter = iter_prime; break; }
                    }while(false);
                }
                if(match){ break; }else{ iter = iter_prime; }
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    do{
                        if(iter != last && *iter == '*'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){ iter = iter_prime; break; }
                    }while(false);
                }
                if(match){ break; }else{ iter = iter_prime; }
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    do{
                        if(iter != last && *iter == '/'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){ iter = iter_prime; break; }
                    }while(false);
                }
                if(!match){ iter = iter_prime; }
            }while(false);
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_bar(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        do{
            if(iter == last){ match = false; }else{
                char c = *iter;
                if(
                    ((c < 'a') || (c > 'z'))
                ){
                    ++iter;
                    match = true;
                }else{ match = false; }
            }
            if(!match){ break; }
            InputIter iter_prime = iter;
            while(iter != last){
                if(iter == last){ match = false; }else{
                    char c = *iter;
                    if(
                        ((c < 'a') || (c > 'z'))
                    ){
                        ++iter;
                        match = true;
                    }else{ match = false; }
                }
                if(match){ iter_prime = iter; }else{
                    iter = iter_prime;
                    match = true;
                    break;
                }
            }
        }while(false);
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_hoge(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            do{
                if(iter != last && *iter == '!'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; break; }
                do{
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == ' '){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(!match){ break; }
                    InputIter iter_prime = iter;
                    while(iter != last){
                        if(iter == last){ match = false; }else{
                            InputIter iter_prime = iter;
                            do{
                                if(iter != last && *iter == ' '){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                            }while(false);
                        }
                        if(match){ iter_prime = iter; }else{
                            iter = iter_prime;
                            match = true;
                            break;
                        }
                    }
                }while(false);
                if(!match){ iter = iter_prime; break; }
            }while(false);
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter, class InsertIter>
    static std::pair<bool, InputIter> tokenize(InputIter first, InputIter last, InsertIter token_inserter){
        InputIter iter = first;
        std::pair<bool, InputIter> result;
        while(iter != last){
            result = reg_foo(iter, last);
            if(result.first){
                *token_inserter = std::make_pair(token_foo, std::make_pair(iter, result.second));
                iter = result.second;
                continue;
            }
            result = reg_bar(iter, last);
            if(result.first){
                *token_inserter = std::make_pair(token_bar, std::make_pair(iter, result.second));
                iter = result.second;
                continue;
            }
            result = reg_hoge(iter, last);
            if(result.first){
                iter = result.second;
                continue;
            }
            break;
        }
        return result;
    }
};
} // namespace aaa

#endif // OFILE_HPP_

