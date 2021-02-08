#ifndef THISPTR_HPP
#define THISPTR_HPP

namespace ct
{
    struct ThisPtr
    {

        Type type;
    };

    template <typename Stream>
    void print(Stream &stream, const ThisPtr & /* this_ptr */)
    {
        stream << "this";
    }

    bool operator==(const ThisPtr &lhs, const ThisPtr &rhs)
    {
        return lhs.type == rhs.type;
    }
    bool operator!=(const ThisPtr &lhs, const ThisPtr &rhs)
    {
        return !(lhs == rhs);
    }

    template <typename Visitor>
    ThisPtr transform(Visitor &&visitor, ThisPtr &&this_ptr)
    {
        return {visitor.visit(std::move(this_ptr.type))};
    }

    const Type * get_type(const ThisPtr& expr) {
        return &expr.type;
    }


} // namespace ct

#endif /* THISPTR_HPP */
