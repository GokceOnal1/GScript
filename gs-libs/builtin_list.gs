obj _LIST_ = {
    field _list_ = [0];
    method len() {
        return __builtin_len__(_list_);
    };
    method get_first() {
        return _list_->0;
    };
    method size() {
        return __builtin_len__(_list_);
    };
    method add(param to_add) {
        return __builtin_add__(_list_, to_add);
    };
};