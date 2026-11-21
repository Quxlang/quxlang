# Algorithms

Module `std` · source files `algorithm_iterators.qxs`, `algorithm_search.qxs`,
`algorithm_transfer.qxs`, `algorithm_ordering.qxs`, `algorithm_ranges.qxs`,
`algorithm_buffered.qxs`

The algorithm functions of module `std` are function templates that operate on
sequences of elements. A sequence is designated by a pair of iterators or by a
range object, so the functions are independent of the container that stores the
elements. The functions accept built-in arrays, the standard containers, and
user-defined iterator types that provide the required operations. The
algorithm functions do not allocate memory; the functions that require
temporary element storage receive that storage from the caller.

This page specifies the sequence model and the argument-passing conventions
that all algorithm functions share, and lists every function with a link to its
specification.

| Page | Contents |
| --- | --- |
| [Iterator utilities](algorithms-iterators.md) | The distance type, the default callable types, `iterator_range`, `subrange`, `back_inserter`, and `scratch_ref`. |
| [Search, comparison, and fold algorithms](algorithms-search.md) | Functions that read a sequence: quantifiers, counting, search, comparison, extrema, and folds. |
| [Transfer and modification algorithms](algorithms-transfer.md) | Functions that write elements: copying, moving, transformation, replacement, removal, reversal, rotation, and partitioning. |
| [Ordering algorithms](algorithms-ordering.md) | Functions that depend on an ordering: heaps, sorting, selection, binary search, merging, multiset operations, and permutations. |
| [Buffered algorithms](algorithms-buffered.md) | Stable reordering functions that use caller-supplied scratch storage. |
| [Range forms](algorithms-ranges.md) | The overloads that receive a range object in place of an iterator pair. |

## Sequence model

### Iterators and sentinels

A sequence is the half-open interval [`first`, `last`): `first` designates the
first element and `last` is the exclusive boundary. An empty sequence has
`first` equal to `last`. The algorithm functions use the following iterator
operations, and each function requires only the operations that its
implementation uses.

| Term | Required operations |
| --- | --- |
| Single-pass input | `first != last`, `first++`, and `first->`. |
| Forward | The single-pass operations, and copies of an iterator denote stable positions that can be traversed again. |
| Bidirectional | The forward operations and `--`. |
| Random access | The forward operations, `last - first` with a signed element distance ([`std::iterator_difference`](algorithms-iterators.md#stditerator_difference)) as the result, and `first + count` for a `count` of type `SZ`. |

The dereference expression `first->` can produce a reference or a value. A
function that assigns, exchanges, or moves elements requires `first->` to
produce a mutable reference.

The boundary `last` is a sentinel. In the declarations that give `last` its own
deduced type (`sentinel_type`, `sentinel1`, `sentinel2`), the type of `last`
can differ from the iterator type, and the only requirement on the pair is the
comparison `first != last`. In the declarations that give `first` and `last`
the same deduced type, both arguments have identical types. The function
sections state the applicable requirement.

Array pointers (`MUT=>>T` and `CONST=>>T`) and the iterators of
[`std::dynarr`](dynarr.md) provide the random-access operations. The iterators
of [`std::list`](list.md) provide the bidirectional operations.

### Ranges

A range is an lvalue `range` for which `range.BEGIN()` and `range.END()` are
valid. Every algorithm function has a [range form](algorithms-ranges.md) that
receives a range through the named argument `@range` (or two ranges through
`@range1` and `@range2`) in place of the iterator pair. The range form evaluates
each boundary once and calls the iterator form. A temporary range is rejected
during compilation.
[`std::subrange`](algorithms-iterators.md#stdsubrange) creates a range object
from two iterators.

### Output iterators

A function that writes to a destination receives an output iterator, which
supports `output-> := value` and `output++`. A destination that is designated
by an ordinary iterator contains a live element at every position that is
written. [`std::back_inserter`](algorithms-iterators.md#stdback_inserter)
creates an output iterator that appends each assigned value to a container.

## Argument-passing conventions

### Named arguments

Every parameter of every algorithm function is a named parameter. A call
supplies each argument in the form `@name expression`, in any order.

```quxlang
AUTO found := std::find(@first values.BEGIN(), @last values.END(), @value 4);
AUTO found_in_range := std::find(@range values, @value 4);
std::sort(@range records, @projection score);
```

The following argument names have the same meaning in every function that
declares them.

| Argument | Meaning |
| --- | --- |
| `@first`, `@last` | The boundaries of the sequence. |
| `@first1`, `@last1`, `@first2`, `@last2` | The boundaries of the first and second sequence of a function with two inputs. |
| `@range`, `@range1`, `@range2` | The range objects of a range form. |
| `@output` | The output iterator of the destination. |
| `@value` | A lookup value or an assigned value. |
| `@predicate` | A callable that tests one projected element and returns `BOOL`. |
| `@equal` | A callable that tests two values for equivalence and returns `BOOL`. |
| `@compare` | A callable that orders two values and returns `ORDER`. |
| `@projection`, `@projection1`, `@projection2` | A callable that maps an element to the value that is tested or compared. |
| `@scratch` | A [`std::scratch_ref`](algorithms-iterators.md#stdscratch_ref) that designates caller-supplied temporary storage. |

Further argument names, such as `@middle`, `@nth`, `@count`, `@operation`,
`@action`, `@generator`, and `@initial`, are specified in the function
sections.

### Optional arguments

The arguments `@equal`, `@compare`, `@projection`, `@projection1`, and
`@projection2` are optional in every function that accepts them. A function
supplies the following default when a call omits the argument.

| Argument | Default |
| --- | --- |
| `@equal` | A [`std::equal_to`](algorithms-iterators.md#stdequal_to) object, which applies `==`. |
| `@compare` | A [`std::compare_three_way`](algorithms-iterators.md#stdcompare_three_way) object, which applies `<=>`. |
| `@projection`, `@projection1`, `@projection2` | A [`std::identity`](algorithms-iterators.md#stdidentity) object, which returns its argument. |

A function with optional arguments declares the sequence boundaries followed by
`@KWARGS:options ...`. The function adds a default object for each absent
optional argument and forwards the complete argument set to a module-private
implementation function with a fixed parameter list. The table that follows
the declarations in each function section lists the parameters of that
implementation function other than the boundaries, and marks each parameter as
required or optional. A call that omits a required argument, or that supplies
an argument name that the implementation function does not declare, is
rejected during compilation.

### Callable arguments

A callable argument is any value that can be invoked with the argument list
that the function section states: a lambda, a function value, or an object of
a type that declares `OPERATOR()`. The callable parameters are declared with
the pattern `TT`, which retains the reference form of the argument. A callable
that is passed as an lvalue is therefore borrowed and invoked in place; the
callable is not copied, and state that its call operator modifies is observable
by the caller after the call.

- A predicate, a projection, a unary `@operation`, and an `@action` receive one
  positional argument.
- `@equal`, `@compare`, and a binary `@operation` receive the named arguments
  `@left` and `@right`.
- The `@operation` of a fold receives the named arguments `@accumulator` and
  `@value`.

A callable with a result type that the function cannot use, such as a
predicate that returns `I32` or a comparator that returns `BOOL`, is rejected
during compilation.

### Projections and lookup values

A projection selects the part of each element that a predicate, an equality
relation, or a comparator examines. The function applies the projection to an
element immediately before each test. The element that a function copies,
moves, exchanges, or returns an iterator to is always the complete element.

A lookup value (`@value`, `@old_value`) is declared `CONST& AUTO` and is passed
to the relation as the `@right` operand without projection. The type of a
lookup value can therefore differ from the element type when the relation
accepts the projected element type and the lookup type.

### Results

A function returns one of the following kinds of result.

- A scalar result of type `BOOL`, `SZ`, or `ORDER`.
- An iterator that designates a position in the input sequence. A search that
  finds no element returns the end position.
- A [`std::iterator_range`](algorithms-iterators.md#stditerator_range) that
  designates a subsequence of the input.
- A composite whose named fields hold the final positions of several cursors,
  such as `in` and `out`.

Most declarations state the return type as `AUTO`; the function sections state
the deduced type.

### Preconditions and exceptions

The boundary `last` is reachable from `first` by repeated `++`, and every
iterator in [`first`, `last`) is dereferenceable. A violation of these
requirements, or of a precondition that a function section states, is
undefined behavior; the functions perform no checks.

The algorithm functions contain no exception handlers. An exception thrown by
a callable, an iterator operation, or an element operation propagates to the
caller.

## Function index

**[Search, comparison, and fold algorithms](algorithms-search.md)**

| Function | Category | Description | Range form |
| --- | --- | --- | --- |
| [`all_of`](algorithms-search.md#all_of) | Quantifiers and counting | Tests whether a predicate holds for every element. | [range form](algorithms-ranges.md#all_of) |
| [`any_of`](algorithms-search.md#any_of) | Quantifiers and counting | Tests whether a predicate holds for at least one element. | [range form](algorithms-ranges.md#any_of) |
| [`none_of`](algorithms-search.md#none_of) | Quantifiers and counting | Tests whether a predicate holds for no element. | [range form](algorithms-ranges.md#none_of) |
| [`count`](algorithms-search.md#count) | Quantifiers and counting | Counts the elements equivalent to a value. | [range form](algorithms-ranges.md#count) |
| [`count_if`](algorithms-search.md#count_if) | Quantifiers and counting | Counts the elements that satisfy a predicate. | [range form](algorithms-ranges.md#count_if) |
| [`find`](algorithms-search.md#find) | Element search | Finds the first element equivalent to a value. | [range form](algorithms-ranges.md#find) |
| [`find_if`](algorithms-search.md#find_if) | Element search | Finds the first element that satisfies a predicate. | [range form](algorithms-ranges.md#find_if) |
| [`find_if_not`](algorithms-search.md#find_if_not) | Element search | Finds the first element that does not satisfy a predicate. | [range form](algorithms-ranges.md#find_if_not) |
| [`find_first_of`](algorithms-search.md#find_first_of) | Element search | Finds the first element equivalent to any element of a second sequence. | [range form](algorithms-ranges.md#find_first_of) |
| [`adjacent_find`](algorithms-search.md#adjacent_find) | Element search | Finds the first pair of equivalent adjacent elements. | [range form](algorithms-ranges.md#adjacent_find) |
| [`min_element`](algorithms-search.md#min_element) | Element search | Finds the first minimum element. | [range form](algorithms-ranges.md#min_element) |
| [`max_element`](algorithms-search.md#max_element) | Element search | Finds the first maximum element. | [range form](algorithms-ranges.md#max_element) |
| [`minmax_element`](algorithms-search.md#minmax_element) | Element search | Finds the first minimum element and the first maximum element. | [range form](algorithms-ranges.md#minmax_element) |
| [`search`](algorithms-search.md#search) | Subsequence search | Finds the first occurrence of a pattern sequence. | [range form](algorithms-ranges.md#search) |
| [`find_end`](algorithms-search.md#find_end) | Subsequence search | Finds the last occurrence of a pattern sequence. | [range form](algorithms-ranges.md#find_end) |
| [`search_n`](algorithms-search.md#search_n) | Subsequence search | Finds the first run of a given number of equivalent elements. | [range form](algorithms-ranges.md#search_n) |
| [`equal`](algorithms-search.md#equal) | Sequence comparison | Tests whether two sequences have equal lengths and equivalent elements. | [range form](algorithms-ranges.md#equal) |
| [`mismatch`](algorithms-search.md#mismatch) | Sequence comparison | Finds the first position at which two sequences differ. | [range form](algorithms-ranges.md#mismatch) |
| [`lexicographical_compare`](algorithms-search.md#lexicographical_compare) | Sequence comparison | Computes the lexicographical three-way order of two sequences. | [range form](algorithms-ranges.md#lexicographical_compare) |
| [`is_permutation`](algorithms-search.md#is_permutation) | Sequence comparison | Tests whether two sequences contain the same elements with the same multiplicities. | [range form](algorithms-ranges.md#is_permutation) |
| [`for_each`](algorithms-search.md#for_each) | Traversal and folds | Invokes a callable for each element. | [range form](algorithms-ranges.md#for_each) |
| [`fold_left`](algorithms-search.md#fold_left) | Traversal and folds | Folds the elements from the first to the last. | [range form](algorithms-ranges.md#fold_left) |
| [`fold_right`](algorithms-search.md#fold_right) | Traversal and folds | Folds the elements from the last to the first. | [range form](algorithms-ranges.md#fold_right) |

**[Transfer and modification algorithms](algorithms-transfer.md)**

| Function | Category | Description | Range form |
| --- | --- | --- | --- |
| [`copy`](algorithms-transfer.md#copy) | Copying and moving | Copies the elements to a destination in traversal order. | [range form](algorithms-ranges.md#copy) |
| [`move`](algorithms-transfer.md#move) | Copying and moving | Moves the elements to a destination in traversal order. | [range form](algorithms-ranges.md#move) |
| [`copy_backward`](algorithms-transfer.md#copy_backward) | Copying and moving | Copies the elements to a destination in reverse traversal order. | [range form](algorithms-ranges.md#copy_backward) |
| [`move_backward`](algorithms-transfer.md#move_backward) | Copying and moving | Moves the elements to a destination in reverse traversal order. | [range form](algorithms-ranges.md#move_backward) |
| [`copy_if`](algorithms-transfer.md#copy_if) | Copying and moving | Copies the elements that satisfy a predicate. | [range form](algorithms-ranges.md#copy_if) |
| [`remove_copy`](algorithms-transfer.md#remove_copy) | Copying and moving | Copies the elements that are not equivalent to a value. | [range form](algorithms-ranges.md#remove_copy) |
| [`remove_copy_if`](algorithms-transfer.md#remove_copy_if) | Copying and moving | Copies the elements that do not satisfy a predicate. | [range form](algorithms-ranges.md#remove_copy_if) |
| [`unique_copy`](algorithms-transfer.md#unique_copy) | Copying and moving | Copies the first element of each run of equivalent adjacent elements. | [range form](algorithms-ranges.md#unique_copy) |
| [`reverse_copy`](algorithms-transfer.md#reverse_copy) | Copying and moving | Copies the elements in reverse order. | [range form](algorithms-ranges.md#reverse_copy) |
| [`rotate_copy`](algorithms-transfer.md#rotate_copy) | Copying and moving | Copies a rotated arrangement of the elements. | [range form](algorithms-ranges.md#rotate_copy) |
| [`partition_copy`](algorithms-transfer.md#partition_copy) | Copying and moving | Copies the elements to one of two destinations according to a predicate. | [range form](algorithms-ranges.md#partition_copy) |
| [`transform`](algorithms-transfer.md#transform) | Transformation and assignment | Writes the result of a unary or binary operation for each element. | [range form](algorithms-ranges.md#transform) |
| [`fill`](algorithms-transfer.md#fill) | Transformation and assignment | Assigns one value to every element. | [range form](algorithms-ranges.md#fill) |
| [`generate`](algorithms-transfer.md#generate) | Transformation and assignment | Assigns successive generator results to the elements. | [range form](algorithms-ranges.md#generate) |
| [`replace`](algorithms-transfer.md#replace) | Transformation and assignment | Replaces the elements equivalent to a value. | [range form](algorithms-ranges.md#replace) |
| [`replace_if`](algorithms-transfer.md#replace_if) | Transformation and assignment | Replaces the elements that satisfy a predicate. | [range form](algorithms-ranges.md#replace_if) |
| [`replace_copy`](algorithms-transfer.md#replace_copy) | Transformation and assignment | Copies the elements and substitutes those equivalent to a value. | [range form](algorithms-ranges.md#replace_copy) |
| [`replace_copy_if`](algorithms-transfer.md#replace_copy_if) | Transformation and assignment | Copies the elements and substitutes those that satisfy a predicate. | [range form](algorithms-ranges.md#replace_copy_if) |
| [`swap_ranges`](algorithms-transfer.md#swap_ranges) | Transformation and assignment | Exchanges the corresponding elements of two sequences. | [range form](algorithms-ranges.md#swap_ranges) |
| [`remove`](algorithms-transfer.md#remove) | Removal | Compacts the elements that are not equivalent to a value. | [range form](algorithms-ranges.md#remove) |
| [`remove_if`](algorithms-transfer.md#remove_if) | Removal | Compacts the elements that do not satisfy a predicate. | [range form](algorithms-ranges.md#remove_if) |
| [`unique`](algorithms-transfer.md#unique) | Removal | Compacts the first element of each run of equivalent adjacent elements. | [range form](algorithms-ranges.md#unique) |
| [`reverse`](algorithms-transfer.md#reverse) | Reordering and partitioning | Reverses the order of the elements. | [range form](algorithms-ranges.md#reverse) |
| [`rotate`](algorithms-transfer.md#rotate) | Reordering and partitioning | Exchanges two adjacent subsequences. | [range form](algorithms-ranges.md#rotate) |
| [`partition`](algorithms-transfer.md#partition) | Reordering and partitioning | Places the elements that satisfy a predicate before the other elements. | [range form](algorithms-ranges.md#partition) |
| [`is_partitioned`](algorithms-transfer.md#is_partitioned) | Reordering and partitioning | Tests whether a sequence is partitioned by a predicate. | [range form](algorithms-ranges.md#is_partitioned) |

**[Ordering algorithms](algorithms-ordering.md)**

| Function | Category | Description | Range form |
| --- | --- | --- | --- |
| [`make_heap`](algorithms-ordering.md#make_heap) | Heap operations | Arranges a sequence into a heap. | [range form](algorithms-ranges.md#make_heap) |
| [`push_heap`](algorithms-ordering.md#push_heap) | Heap operations | Extends a heap by the final element of the sequence. | [range form](algorithms-ranges.md#push_heap) |
| [`pop_heap`](algorithms-ordering.md#pop_heap) | Heap operations | Moves the maximum element of a heap to the final position. | [range form](algorithms-ranges.md#pop_heap) |
| [`sort_heap`](algorithms-ordering.md#sort_heap) | Heap operations | Converts a heap into a sorted sequence. | [range form](algorithms-ranges.md#sort_heap) |
| [`is_heap_until`](algorithms-ordering.md#is_heap_until) | Heap operations | Finds the first element that violates the heap property. | [range form](algorithms-ranges.md#is_heap_until) |
| [`is_heap`](algorithms-ordering.md#is_heap) | Heap operations | Tests whether a sequence is a heap. | [range form](algorithms-ranges.md#is_heap) |
| [`sort`](algorithms-ordering.md#sort) | Sorting and selection | Sorts a sequence. | [range form](algorithms-ranges.md#sort) |
| [`nth_element`](algorithms-ordering.md#nth_element) | Sorting and selection | Places one element at its sorted position and partitions the sequence around it. | [range form](algorithms-ranges.md#nth_element) |
| [`partial_sort`](algorithms-ordering.md#partial_sort) | Sorting and selection | Sorts the smallest elements into a prefix of the sequence. | [range form](algorithms-ranges.md#partial_sort) |
| [`partial_sort_copy`](algorithms-ordering.md#partial_sort_copy) | Sorting and selection | Copies the smallest elements into a bounded destination in sorted order. | [range form](algorithms-ranges.md#partial_sort_copy) |
| [`is_sorted_until`](algorithms-ordering.md#is_sorted_until) | Sorting and selection | Finds the first element that precedes its predecessor. | [range form](algorithms-ranges.md#is_sorted_until) |
| [`is_sorted`](algorithms-ordering.md#is_sorted) | Sorting and selection | Tests whether a sequence is sorted. | [range form](algorithms-ranges.md#is_sorted) |
| [`lower_bound`](algorithms-ordering.md#lower_bound) | Binary search | Finds the first element that does not precede a value. | [range form](algorithms-ranges.md#lower_bound) |
| [`upper_bound`](algorithms-ordering.md#upper_bound) | Binary search | Finds the first element that a value precedes. | [range form](algorithms-ranges.md#upper_bound) |
| [`equal_range`](algorithms-ordering.md#equal_range) | Binary search | Finds the subsequence of elements equal to a value. | [range form](algorithms-ranges.md#equal_range) |
| [`binary_search`](algorithms-ordering.md#binary_search) | Binary search | Tests whether a sorted sequence contains a value. | [range form](algorithms-ranges.md#binary_search) |
| [`partition_point`](algorithms-ordering.md#partition_point) | Binary search | Finds the boundary of a partitioned sequence. | [range form](algorithms-ranges.md#partition_point) |
| [`merge`](algorithms-ordering.md#merge) | Merging and multiset operations | Merges two sorted sequences. | [range form](algorithms-ranges.md#merge) |
| [`set_union`](algorithms-ordering.md#set_union) | Merging and multiset operations | Writes the multiset union of two sorted sequences. | [range form](algorithms-ranges.md#set_union) |
| [`set_intersection`](algorithms-ordering.md#set_intersection) | Merging and multiset operations | Writes the multiset intersection of two sorted sequences. | [range form](algorithms-ranges.md#set_intersection) |
| [`set_difference`](algorithms-ordering.md#set_difference) | Merging and multiset operations | Writes the multiset difference of two sorted sequences. | [range form](algorithms-ranges.md#set_difference) |
| [`set_symmetric_difference`](algorithms-ordering.md#set_symmetric_difference) | Merging and multiset operations | Writes the multiset symmetric difference of two sorted sequences. | [range form](algorithms-ranges.md#set_symmetric_difference) |
| [`includes`](algorithms-ordering.md#includes) | Merging and multiset operations | Tests whether one sorted sequence contains another as a multiset. | [range form](algorithms-ranges.md#includes) |
| [`next_permutation`](algorithms-ordering.md#next_permutation) | Permutations | Rearranges a sequence into the next permutation. | [range form](algorithms-ranges.md#next_permutation) |
| [`prev_permutation`](algorithms-ordering.md#prev_permutation) | Permutations | Rearranges a sequence into the preceding permutation. | [range form](algorithms-ranges.md#prev_permutation) |

**[Buffered algorithms](algorithms-buffered.md)**

| Function | Category | Description | Range form |
| --- | --- | --- | --- |
| [`stable_partition`](algorithms-buffered.md#stable_partition) | Buffered algorithms | Partitions a sequence by a predicate and preserves the relative order within both groups. | [range form](algorithms-ranges.md#stable_partition) |
| [`inplace_merge`](algorithms-buffered.md#inplace_merge) | Buffered algorithms | Merges two adjacent sorted subsequences and preserves the relative order of equal elements. | [range form](algorithms-ranges.md#inplace_merge) |
| [`stable_sort`](algorithms-buffered.md#stable_sort) | Buffered algorithms | Sorts a sequence and preserves the relative order of equal elements. | [range form](algorithms-ranges.md#stable_sort) |

The functions `std::subrange` and `std::back_inserter` and the supporting types
are specified in [Iterator utilities](algorithms-iterators.md).

## See also

- [`std::dynarr`](dynarr.md)
- [`std::list`](list.md)
- [`std::geometric_array`](geometric-array.md)
- [Call arguments](../../reference/call-arguments.md)
- [Lambdas](../../reference/lambdas.md)
- [Comparison operators](../../reference/comparison-operators.md)
- [Type queries and deduction](../../reference/type-queries-and-deduction.md)
