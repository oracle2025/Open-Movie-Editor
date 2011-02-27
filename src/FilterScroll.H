#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>


namespace nle
{

class FilterClip;
class FilterBase;

class FilterScroll : public Fl_Scroll {
    FilterClip* m_clip;
    int nchild;
    int nchildheight;
    Fl_Pack* m_pack;
public:
    FilterScroll(int X, int Y, int W, int H, const char* L=0);
    void resize(int X, int Y, int W, int H);
    void AddItem(FilterBase* filter, FilterClip* clip);
    void setClip( FilterClip* clip );
    FilterClip* getClip();
};

} /* namespace nle */
