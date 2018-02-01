#include <rttr/registration>
#include <rttr/type>
#include <string>
struct point
{
    int x;
    int y;
};
struct foo
{
    point get_point();
};
#undef class_
RTTR_REGISTRATION
{
    rttr::registration::class_<foo>("foo").method("bar", &foo::get_point);
}