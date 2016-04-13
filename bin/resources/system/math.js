var math = {}

math.max = function (a, b) {
    return a > b ? a : b;
}
math.min = function (a, b) {
    return a < b ? a : b;
}
math.clamp = function (a, b, c) {
    return math.max(b, math.min(c, a));
}