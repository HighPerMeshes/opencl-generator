#ifndef NULLSTMT_HPP
#define NULLSTMT_HPP

namespace ct
{
    struct NullStmt
    {
    };

    template <typename Stream>
    void print(Stream &stream, const NullStmt & /* null_stmt */)
    {
    }

    bool operator==(const NullStmt & /* null_stmt */, const NullStmt & /* null_stmt */)
    {
        return true;
    }
    bool operator!=(const NullStmt & /* null_stmt */, const NullStmt & /* null_stmt */)
    {
        return false;
    }

    constexpr NullStmt Null = {};

    const Type *get_type(const NullStmt &)
    {
        return nullptr;
    }

} // namespace ct

#endif /* NULLSTMT_HPP */
