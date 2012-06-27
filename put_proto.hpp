#ifndef PUT_PROTO_HPP_
#define PUT_PROTO_HPP_

#include <utility>
#include <iterator>
#include <cstring>

namespace test{

enum token{
    token_t,
    token_u
};

class lexer{
public:
    template<class InputIter>
    static std::pair<bool, InputIter> reg_t(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{ 
            InputIter iter_prime = iter;
            do{
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    do{
                        {
                            InputIter iter_prime = iter;
                            while(iter != last){
                                if(iter != last && *iter == 'a'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(match){ iter_prime = iter; }else{
                                    iter = iter_prime;
                                    match = true;
                                    break;
                                }
                            }
                        }
                        if(!match){ iter = iter_prime; break; }
                        do{
                            if(iter != last && *iter == 'b'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ break; }
                            InputIter iter_prime = iter;
                            while(iter != last){
                                if(iter != last && *iter == 'b'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(match){ iter_prime = iter; }else{
                                    iter = iter_prime;
                                    match = true;
                                    break;
                                }
                            }
                        }while(false);
                        if(!match){ iter = iter_prime; break; }
                        if(iter != last && *iter == 'c'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        match = true;
                        if(!match){ iter = iter_prime; break; }
                    }while(false);
                }
                if(match){ break; }else{ iter = iter_prime; }
                if(iter == first){ match = true; }else{
                    InputIter iter_prime = iter;
                    --iter_prime;
                    match = *iter_prime == '\n';
                }
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    if(iter == last){ match = false; }else{
                        InputIter iter_prime = iter;
                        do{
                            if(iter != last && *iter == 'd'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                            if(iter != last && *iter == 'e'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                            if(iter != last && *iter == 'f'){
                                ++iter;
                                match = true;
                            }else{ match = false; }
                            if(!match){ iter = iter_prime; break; }
                        }while(false);
                    }
                    if(match){
                        if(iter == last){ match = false; }else{
                            InputIter iter_prime = iter;
                            do{
                                if(iter != last && *iter == 'g'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                                if(iter != last && *iter == 'h'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                                if(iter != last && *iter == 'i'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                            }while(false);
                        }
                        InputIter iter_prime_prime = iter;
                        if(match){ iter = iter_prime_prime; }else{ iter = iter_prime; }
                    }
                }
                if(!match){ iter = iter_prime; }
            }while(false);
        }
        return std::make_pair(match, iter);
    }

    template<class InputIter>
    static std::pair<bool, InputIter> reg_u(InputIter first, InputIter last){
        InputIter iter = first;
        bool match = true;
        if(iter == last){ match = false; }else{
            InputIter iter_prime = iter;
            do{
                if(iter != last && *iter == 'g'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; break; }
                if(iter != last && *iter == 'h'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; break; }
                if(iter != last && *iter == 'i'){
                    ++iter;
                    match = true;
                }else{ match = false; }
                if(!match){ iter = iter_prime; break; }
                if(iter == last){ match = false; }else{ 
                    InputIter iter_prime = iter;
                    do{
                        if(iter == last){ match = false; }else{
                            InputIter iter_prime = iter;
                            do{
                                if(iter != last && *iter == 'f'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                                if(iter != last && *iter == 'o'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                                if(iter != last && *iter == 'o'){
                                    ++iter;
                                    match = true;
                                }else{ match = false; }
                                if(!match){ iter = iter_prime; break; }
                            }while(false);
                        }
                        if(match){ break; }else{ iter = iter_prime; }
                        if(iter != last && std::isalnum(*iter)){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(match){ break; }else{ iter = iter_prime; }
                        if(iter == last){ match = false; }else{ 
                            InputIter iter_prime = iter;
                            do{
                                if(iter == last){ match = false; }else{
                                    InputIter iter_prime = iter;
                                    do{
                                        {
                                            InputIter iter_prime = iter;
                                            while(iter != last){
                                                if(iter != last && *iter == 'a'){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                                if(match){ iter_prime = iter; }else{
                                                    iter = iter_prime;
                                                    match = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if(!match){ iter = iter_prime; break; }
                                        do{
                                            if(iter != last && *iter == 'b'){
                                                ++iter;
                                                match = true;
                                            }else{ match = false; }
                                            if(!match){ break; }
                                            InputIter iter_prime = iter;
                                            while(iter != last){
                                                if(iter != last && *iter == 'b'){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                                if(match){ iter_prime = iter; }else{
                                                    iter = iter_prime;
                                                    match = true;
                                                    break;
                                                }
                                            }
                                        }while(false);
                                        if(!match){ iter = iter_prime; break; }
                                        if(iter != last && *iter == 'c'){
                                            ++iter;
                                            match = true;
                                        }else{ match = false; }
                                        match = true;
                                        if(!match){ iter = iter_prime; break; }
                                    }while(false);
                                }
                                if(match){ break; }else{ iter = iter_prime; }
                                if(iter == first){ match = true; }else{
                                    InputIter iter_prime = iter;
                                    --iter_prime;
                                    match = *iter_prime == '\n';
                                }
                                if(iter == last){ match = false; }else{
                                    InputIter iter_prime = iter;
                                    if(iter == last){ match = false; }else{
                                        InputIter iter_prime = iter;
                                        do{
                                            if(iter != last && *iter == 'd'){
                                                ++iter;
                                                match = true;
                                            }else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                            if(iter != last && *iter == 'e'){
                                                ++iter;
                                                match = true;
                                            }else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                            if(iter != last && *iter == 'f'){
                                                ++iter;
                                                match = true;
                                            }else{ match = false; }
                                            if(!match){ iter = iter_prime; break; }
                                        }while(false);
                                    }
                                    if(match){
                                        if(iter == last){ match = false; }else{
                                            InputIter iter_prime = iter;
                                            do{
                                                if(iter != last && *iter == 'g'){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                                if(!match){ iter = iter_prime; break; }
                                                if(iter != last && *iter == 'h'){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                                if(!match){ iter = iter_prime; break; }
                                                if(iter != last && *iter == 'i'){
                                                    ++iter;
                                                    match = true;
                                                }else{ match = false; }
                                                if(!match){ iter = iter_prime; break; }
                                            }while(false);
                                        }
                                        InputIter iter_prime_prime = iter;
                                        if(match){ iter = iter_prime_prime; }else{ iter = iter_prime; }
                                    }
                                }
                                if(!match){ iter = iter_prime; }
                            }while(false);
                        }
                        if(!match){ iter = iter_prime; }
                    }while(false);
                }
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
            result = reg_t(iter, last);
            if(result.first){
                *token_inserter = std::make_pair(token_t, std::make_pair(iter, result.second));
                iter = result.second;
                continue;
            }
            result = reg_u(iter, last);
            if(result.first){
                *token_inserter = std::make_pair(token_u, std::make_pair(iter, result.second));
                iter = result.second;
                continue;
            }
            break;
        }
        return result;
    }
};
} // namespace test

#endif // PUT_PROTO_HPP_

