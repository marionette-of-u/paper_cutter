template<class InputIter>
std::pair<bool, InputIter> reg_test(InputIter first, InputIter last){
    InputIter iter = first;
    bool match = true;
    if(iter == last){ match = false; }else{
        InputIter iter_prime = iter;
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
                        if(!match){
                            iter = iter_prime;
                            break;
                        }
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
                        if(!match){
                            iter = iter_prime;
                            break;
                        }
                        if(iter != last && *iter == 'c'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        match = true;
                        if(!match){
                            iter = iter_prime;
                            break;
                        }
                    }while(false);
                }
                if(match){ break; }else{ iter = iter_prime; }
                if(iter == last){ match = false; }else{
                    InputIter iter_prime = iter;
                    do{
                        if(iter != last && *iter == 'd'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){
                            iter = iter_prime;
                            break;
                        }
                        if(iter != last && *iter == 'e'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){
                            iter = iter_prime;
                            break;
                        }
                        if(iter != last && *iter == 'f'){
                            ++iter;
                            match = true;
                        }else{ match = false; }
                        if(!match){
                            iter = iter_prime;
                            break;
                        }
                    }while(false);
                }
                if(!match){ iter = iter_prime; }
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
                    if(!match){
                        iter = iter_prime;
                        break;
                    }
                    if(iter != last && *iter == 'h'){
                        ++iter;
                        match = true;
                    }else{ match = false; }
                    if(!match){
                        iter = iter_prime;
                        break;
                    }
                    if(iter != last && *iter == 'i'){
                        ++iter;
                        match = true;
                    }else{ match = false; }
                    if(!match){
                        iter = iter_prime;
                        break;
                    }
                }while(false);
            }
            InputIter iter_prime_prime = iter;
            if(match){ iter = iter_prime_prime; }else{ iter = iter_prime; }
        }
    }
    return std::make_pair(match, iter);
}
