
// yk::get<T>(yk::variant_view{})
// yk::get<T>(std::variant{})
// yk::get<T>(boost::variant{})
// の実装を置くヘッダ。
//
// 追加の要件として、
//
// std::get_if (https://en.cppreference.com/w/cpp/utility/variant/get_if)
//  と
// boost::variantのboost::get （ポインタに対するオーバーロード）
//
// を同じインターフェース (yk::get<T>(......)) で呼べるようにラップしてほしい
