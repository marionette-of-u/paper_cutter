#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <cstdlib>
#include <cassert>
#include <vector>

namespace parser{

enum token{
    token_symbol_or,
    token_symbol_star,
    token_symbol_plus,
    token_symbol_question,
    token_symbol_left_pare,
    token_symbol_right_pare,
    token_symbol_left_brace,
    token_symbol_right_brace,
    token_symbol_dot,
    token_symbol_eos,
    token_symbol_slash,
    token_symbol_backslash,
    token_symbol_set_left_bracket,
    token_symbol_hat,
    token_symbol_set_right_bracket,
    token_symbol_minus,
    token_symbol_comma,
    token_symbol_colon,
    token_symbol_double_quote,
    token_symbol_any_non_metacharacter,
    token_symbol_number,
    token_symbol_space,
    token_symbol_equal,
    token_0
};

template<class T, int StackSize>
class stack{
public:
    stack(){ gap_ = 0; }
    ~stack(){}
    
    void reset_tmp()
    {
        gap_ = stack_.size();
        tmp_.clear();
    }

    void commit_tmp()
    {
        // may throw
        stack_.reserve(gap_ + tmp_.size());

        // expect not to throw
        stack_.erase(stack_.begin() + gap_, stack_.end());
        stack_.insert(stack_.end(), tmp_.begin(), tmp_.end());
    }

    bool push(const T& f)
    {
        if(StackSize != 0 && StackSize <= stack_.size() + tmp_.size()){
            return false;
        }
        tmp_.push_back(f);
        return true;
    }

    void pop(std::size_t n)
    {
        if(tmp_.size() < n){
            n -= tmp_.size();
            tmp_.clear();
            gap_ -= n;
        }else{
            tmp_.erase(tmp_.end() - n, tmp_.end());
        }
    }

    const T& top()
    {
        if(!tmp_.empty()){
            return tmp_.back();
        } else {
            return stack_[gap_ - 1];
        }
    }

    const T& get_arg(std::size_t base, std::size_t index)
    {
        std::size_t n = tmp_.size();
        if(base - index <= n){
            return tmp_[n - (base - index)];
        }else{
            return stack_[gap_ - (base - n) + index];
        }
    }

    void clear()
    {
        stack_.clear();
    }

private:
    std::vector<T> stack_;
    std::vector<T> tmp_;
    std::size_t gap_;

};

template<class Value, class SemanticAction, int StackSize = 0>
class parser {
public:
    typedef token token_type;
    typedef Value value_type;

public:
    parser(SemanticAction& sa) : sa_(sa){ reset(); }

    void reset()
    {
        error_ = false;
        accepted_ = false;
        clear_stack();
        reset_tmp_stack();
        if(push_stack(&parser::state_0, &parser::gotof_0, value_type())){
            commit_tmp_stack();
        }else{
            sa_.stack_overflow();
            error_ = true;
        }
    }

    bool post(token_type token, const value_type& value)
    {
        assert(!error_);
        reset_tmp_stack();
        while((this->*(stack_top()->state))(token, value)); // may throw
        if(!error_){
            commit_tmp_stack();
        }
        return accepted_ || error_;
    }

    bool accept(value_type& v)
    {
        assert(accepted_);
        if(error_){ return false; }
        v = accepted_value_;
        return true;
    }

    bool error(){ return error_; }

private:
    typedef parser<Value, SemanticAction, StackSize> self_type;
    typedef bool (self_type::*state_type)(token_type, const value_type&);
    typedef bool ( self_type::*gotof_type )(int, const value_type&);

    bool            accepted_;
    bool            error_;
    value_type      accepted_value_;
    SemanticAction& sa_;

    struct stack_frame{
        state_type state;
        gotof_type gotof;
        value_type value;

        stack_frame(state_type s, gotof_type g, const value_type& v)
            : state(s), gotof(g), value(v){}
    };

    stack<stack_frame, StackSize> stack_;
    bool push_stack(state_type s, gotof_type g, const value_type& v)
    {
        bool f = stack_.push(stack_frame(s, g, v));
        assert(!error_);
        if(!f){
            error_ = true;
            sa_.stack_overflow();
        }
        return f;
    }

    void pop_stack(std::size_t n)
    {
        stack_.pop(n);
    }

    const stack_frame* stack_top()
    {
        return &stack_.top();
    }

    const value_type& get_arg(std::size_t base, std::size_t index)
    {
        return stack_.get_arg(base, index).value;
    }

    void clear_stack()
    {
        stack_.clear();
    }

    void reset_tmp_stack()
    {
        stack_.reset_tmp();
    }

    void commit_tmp_stack()
    {
        stack_.commit_tmp();
    }

    bool call_0_make_line(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        std::string arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        std::string arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        std::string r = sa_.make_line(arg0, arg1);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_reg_char(int nonterminal_index, int base, int arg_index0)
    {
        char arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        std::string r = sa_.make_reg_char(arg0);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_reg_sequence(int nonterminal_index, int base, int arg_index0)
    {
        std::string arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        std::string r = sa_.make_reg_sequence(arg0);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_1_make_reg_sequence(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        std::string arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        std::string arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        std::string r = sa_.make_reg_sequence(arg0, arg1);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_0_make_reg_name(int nonterminal_index, int base, int arg_index0)
    {
        char arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        std::string r = sa_.make_reg_name(arg0);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool call_1_make_reg_name(int nonterminal_index, int base, int arg_index0, int arg_index1)
    {
        std::string arg0; sa_.downcast(arg0, get_arg(base, arg_index0));
        char arg1; sa_.downcast(arg1, get_arg(base, arg_index1));
        std::string r = sa_.make_reg_name(arg0, arg1);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        return (this->*(stack_top()->gotof))(nonterminal_index, v);
    }

    bool gotof_0(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 2: return push_stack(&parser::state_58, &parser::gotof_58, v);
        case 4: return push_stack(&parser::state_49, &parser::gotof_49, v);
        default: assert(0); return false;
        }
    }

    bool state_0(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_any_non_metacharacter:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_number:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_space:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_0:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_1(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 6: return push_stack(&parser::state_48, &parser::gotof_48, v);
        default: assert(0); return false;
        }
    }

    bool state_1(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
            // shift
            push_stack(&parser::state_2, &parser::gotof_2, value);
            return false;
        case token_symbol_star:
            // shift
            push_stack(&parser::state_3, &parser::gotof_3, value);
            return false;
        case token_symbol_plus:
            // shift
            push_stack(&parser::state_4, &parser::gotof_4, value);
            return false;
        case token_symbol_question:
            // shift
            push_stack(&parser::state_5, &parser::gotof_5, value);
            return false;
        case token_symbol_left_pare:
            // shift
            push_stack(&parser::state_6, &parser::gotof_6, value);
            return false;
        case token_symbol_right_pare:
            // shift
            push_stack(&parser::state_7, &parser::gotof_7, value);
            return false;
        case token_symbol_left_brace:
            // shift
            push_stack(&parser::state_8, &parser::gotof_8, value);
            return false;
        case token_symbol_right_brace:
            // shift
            push_stack(&parser::state_9, &parser::gotof_9, value);
            return false;
        case token_symbol_dot:
            // shift
            push_stack(&parser::state_10, &parser::gotof_10, value);
            return false;
        case token_symbol_eos:
            // shift
            push_stack(&parser::state_11, &parser::gotof_11, value);
            return false;
        case token_symbol_slash:
            // shift
            push_stack(&parser::state_12, &parser::gotof_12, value);
            return false;
        case token_symbol_backslash:
            // shift
            push_stack(&parser::state_13, &parser::gotof_13, value);
            return false;
        case token_symbol_set_left_bracket:
            // shift
            push_stack(&parser::state_14, &parser::gotof_14, value);
            return false;
        case token_symbol_hat:
            // shift
            push_stack(&parser::state_15, &parser::gotof_15, value);
            return false;
        case token_symbol_set_right_bracket:
            // shift
            push_stack(&parser::state_16, &parser::gotof_16, value);
            return false;
        case token_symbol_minus:
            // shift
            push_stack(&parser::state_17, &parser::gotof_17, value);
            return false;
        case token_symbol_comma:
            // shift
            push_stack(&parser::state_18, &parser::gotof_18, value);
            return false;
        case token_symbol_colon:
            // shift
            push_stack(&parser::state_19, &parser::gotof_19, value);
            return false;
        case token_symbol_double_quote:
            // shift
            push_stack(&parser::state_20, &parser::gotof_20, value);
            return false;
        case token_symbol_any_non_metacharacter:
            // shift
            push_stack(&parser::state_21, &parser::gotof_21, value);
            return false;
        case token_symbol_number:
            // shift
            push_stack(&parser::state_22, &parser::gotof_22, value);
            return false;
        case token_symbol_space:
            // shift
            push_stack(&parser::state_23, &parser::gotof_23, value);
            return false;
        case token_symbol_equal:
            // shift
            push_stack(&parser::state_24, &parser::gotof_24, value);
            return false;
        case token_0:
            return call_0_make_line(2, 6, 1, 5);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_2(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_2(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_3(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_3(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_4(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_4(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_5(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_5(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_6(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_6(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_7(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_7(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_8(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_8(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_9(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_9(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_10(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_10(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_11(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_11(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_12(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_12(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_13(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_13(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_14(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_14(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_15(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_15(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_16(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_16(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_17(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_17(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_18(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_18(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_19(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_19(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_20(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_20(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_21(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_21(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_22(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_22(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_23(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_23(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_24(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_24(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(6, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_25(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 3: return push_stack(&parser::state_47, &parser::gotof_47, v);
        case 5: return push_stack(&parser::state_1, &parser::gotof_1, v);
        default: assert(0); return false;
        }
    }

    bool state_25(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
            // shift
            push_stack(&parser::state_26, &parser::gotof_26, value);
            return false;
        case token_symbol_star:
            // shift
            push_stack(&parser::state_27, &parser::gotof_27, value);
            return false;
        case token_symbol_plus:
            // shift
            push_stack(&parser::state_28, &parser::gotof_28, value);
            return false;
        case token_symbol_question:
            // shift
            push_stack(&parser::state_29, &parser::gotof_29, value);
            return false;
        case token_symbol_left_pare:
            // shift
            push_stack(&parser::state_30, &parser::gotof_30, value);
            return false;
        case token_symbol_right_pare:
            // shift
            push_stack(&parser::state_31, &parser::gotof_31, value);
            return false;
        case token_symbol_left_brace:
            // shift
            push_stack(&parser::state_32, &parser::gotof_32, value);
            return false;
        case token_symbol_right_brace:
            // shift
            push_stack(&parser::state_33, &parser::gotof_33, value);
            return false;
        case token_symbol_dot:
            // shift
            push_stack(&parser::state_34, &parser::gotof_34, value);
            return false;
        case token_symbol_eos:
            // shift
            push_stack(&parser::state_35, &parser::gotof_35, value);
            return false;
        case token_symbol_slash:
            // shift
            push_stack(&parser::state_36, &parser::gotof_36, value);
            return false;
        case token_symbol_backslash:
            // shift
            push_stack(&parser::state_37, &parser::gotof_37, value);
            return false;
        case token_symbol_set_left_bracket:
            // shift
            push_stack(&parser::state_38, &parser::gotof_38, value);
            return false;
        case token_symbol_hat:
            // shift
            push_stack(&parser::state_39, &parser::gotof_39, value);
            return false;
        case token_symbol_set_right_bracket:
            // shift
            push_stack(&parser::state_40, &parser::gotof_40, value);
            return false;
        case token_symbol_minus:
            // shift
            push_stack(&parser::state_41, &parser::gotof_41, value);
            return false;
        case token_symbol_comma:
            // shift
            push_stack(&parser::state_42, &parser::gotof_42, value);
            return false;
        case token_symbol_colon:
            // shift
            push_stack(&parser::state_43, &parser::gotof_43, value);
            return false;
        case token_symbol_double_quote:
            // shift
            push_stack(&parser::state_44, &parser::gotof_44, value);
            return false;
        case token_symbol_any_non_metacharacter:
            // shift
            push_stack(&parser::state_45, &parser::gotof_45, value);
            return false;
        case token_symbol_number:
            // shift
            push_stack(&parser::state_46, &parser::gotof_46, value);
            return false;
        case token_symbol_space:
            // shift
            push_stack(&parser::state_57, &parser::gotof_57, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_26(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_26(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_27(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_27(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_28(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_28(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_29(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_29(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_30(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_30(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_31(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_31(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_32(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_32(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_33(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_33(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_34(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_34(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_35(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_35(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_36(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_36(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_37(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_37(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_38(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_38(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_39(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_39(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_40(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_40(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_41(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_41(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_42(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_42(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_43(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_43(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_44(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_44(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_45(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_45(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_46(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_46(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_char(3, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_47(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_47(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_0_make_reg_sequence(5, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_48(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_48(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
        case token_symbol_star:
        case token_symbol_plus:
        case token_symbol_question:
        case token_symbol_left_pare:
        case token_symbol_right_pare:
        case token_symbol_left_brace:
        case token_symbol_right_brace:
        case token_symbol_dot:
        case token_symbol_eos:
        case token_symbol_slash:
        case token_symbol_backslash:
        case token_symbol_set_left_bracket:
        case token_symbol_hat:
        case token_symbol_set_right_bracket:
        case token_symbol_minus:
        case token_symbol_comma:
        case token_symbol_colon:
        case token_symbol_double_quote:
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
        case token_0:
            return call_1_make_reg_sequence(5, 2, 0, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_49(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 1: return push_stack(&parser::state_50, &parser::gotof_50, v);
        default: assert(0); return false;
        }
    }

    bool state_49(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_any_non_metacharacter:
            // shift
            push_stack(&parser::state_51, &parser::gotof_51, value);
            return false;
        case token_symbol_space:
            // shift
            push_stack(&parser::state_57, &parser::gotof_57, value);
            return false;
        case token_0:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(2, value_type());
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_50(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 4: return push_stack(&parser::state_55, &parser::gotof_55, v);
        default: assert(0); return false;
        }
    }

    bool state_50(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_any_non_metacharacter:
            // shift
            push_stack(&parser::state_52, &parser::gotof_52, value);
            return false;
        case token_symbol_number:
            // shift
            push_stack(&parser::state_53, &parser::gotof_53, value);
            return false;
        case token_symbol_space:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_equal:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_51(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_51(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
            return call_0_make_reg_name(1, 1, 0);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_52(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_52(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
            return call_1_make_reg_name(1, 2, 0, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_53(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_53(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_any_non_metacharacter:
        case token_symbol_number:
        case token_symbol_space:
        case token_symbol_equal:
            return call_1_make_reg_name(1, 2, 0, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_54(int nonterminal_index, const value_type& v)
    {
        switch(nonterminal_index){
        case 4: return push_stack(&parser::state_25, &parser::gotof_25, v);
        default: assert(0); return false;
        }
    }

    bool state_54(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_star:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_plus:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_question:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_left_pare:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_right_pare:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_left_brace:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_right_brace:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_dot:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_eos:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_slash:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_backslash:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_set_left_bracket:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_hat:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_set_right_bracket:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_minus:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_comma:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_colon:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_double_quote:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_any_non_metacharacter:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_number:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_space:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_equal:
            // reduce
                // run_semantic_action();
                pop_stack(0);
                return (this->*(stack_top()->gotof))(4, value_type());
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_55(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_55(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_space:
            // shift
            push_stack(&parser::state_57, &parser::gotof_57, value);
            return false;
        case token_symbol_equal:
            // shift
            push_stack(&parser::state_54, &parser::gotof_54, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_56(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_56(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_star:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_plus:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_question:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_left_pare:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_right_pare:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_left_brace:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_right_brace:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_dot:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_eos:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_slash:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_backslash:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_set_left_bracket:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_hat:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_set_right_bracket:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_minus:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_comma:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_colon:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_double_quote:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_any_non_metacharacter:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_number:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_space:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_equal:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_0:
            // reduce
                // run_semantic_action();
                pop_stack(1);
                return (this->*(stack_top()->gotof))(4, value_type());
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_57(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_57(token_type token, const value_type& value)
    {
        switch(token){
        case token_symbol_or:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_star:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_plus:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_question:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_left_pare:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_right_pare:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_left_brace:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_right_brace:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_dot:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_eos:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_slash:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_backslash:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_set_left_bracket:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_hat:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_set_right_bracket:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_minus:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_comma:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_colon:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_double_quote:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_any_non_metacharacter:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_number:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_space:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_symbol_equal:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        case token_0:
            // reduce
                // run_semantic_action();
                pop_stack(2);
                return (this->*(stack_top()->gotof))(4, value_type());
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    bool gotof_58(int nonterminal_index, const value_type& v)
    {
        assert(0);
        return true;
    }

    bool state_58(token_type token, const value_type& value)
    {
        switch(token){
        case token_0:
            // accept
            // run_semantic_action();
            accepted_ = true;
            accepted_value_ = get_arg(1, 0);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

};

} // namespace parser

#endif // PARSER_HPP_
