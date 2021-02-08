#ifndef FOR_EACH_INTERLEAVE_HPP
#define FOR_EACH_INTERLEAVE_HPP

#include <algorithm>
 
template<
    typename Iterator,
    typename EntryOp,
    typename InterleaveOp
> void for_each_interleave(Iterator&& begin, Iterator&& end, EntryOp&& entry_op, InterleaveOp&& interleave_op) {

    using namespace std;

    if(begin == end) return;

    entry_op(*begin);

    for_each(next(forward<Iterator>(begin)), forward<Iterator>(end), [&entry_op, &interleave_op](auto&& entry){
        interleave_op();
        entry_op(forward<decltype(entry)>(entry));
    });

}

template<
    typename Collection,
    typename EntryOp,
    typename InterleaveOp
> void for_each_interleave(Collection&& collection, EntryOp&& entry_op, InterleaveOp&& interleave_op) {
    using namespace std;
    for_each_interleave(
        begin(forward<Collection>(collection)),
        end(forward<Collection>(collection)),
        forward<EntryOp>(entry_op),
        forward<InterleaveOp>(interleave_op));
}


#endif /* FOR_EACH_INTERLEAVE_HPP */
