// RUN: %metal main 2>&1 | %check

var<private> x: i32;
var<private> y: i32;

// CHECK: int f\(int parameter\d\)
fn f() -> i32
{
    //CHECK: parameter\d
    _ = y;
    return 0;
}

// CHECK: int g\(\)
fn g() -> i32
{
    let y = 42;
    return 0;
}

// CHECK: int h\(int parameter\d\)
fn h() -> i32
{
    _ = y;
    return 0;
}

// CHECK: int i\(int parameter\d\)
fn i() -> i32
{
    // CHECK: f\(parameter\d\)
    _ = f();
}

@compute @workgroup_size(1)
fn main()
{
    // CHECK: int local\d;
    // CHECK: int local\d;
    _ = x;

    // CHECK: f\(local\d\)
    _ = f();

    // CHECK: g\(\)
    _ = g();

    // CHECK: h\(local\d\)
    _ = h();

    // CHECK: i\(local\d\)
    _ = i();
}
