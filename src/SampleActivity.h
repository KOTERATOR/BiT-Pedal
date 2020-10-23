#include "GFX/Activity.h"
#include "GFX/Animator.h"
#include "Views/ButtonView.h"
#include "Views/PotentiometerView.h"

class SampleActivity : public Activity
{
private:
    ButtonView header = ButtonView("HEADER");
    Layout body = Layout(LayoutMode::VERTICAL, Position(0, 0), Size(160, 120), ContainerMode::CENTER);
    Layout footer = Layout(LayoutMode::HORIZONTAL, Position(0, 0), Size(320, 75), ContainerMode::CENTER);
public:
    SampleActivity() : Activity()
    {
        
        int n = 50;
        setSpacing(10);
        body.setSpacing(5);
        ButtonView * labels = new ButtonView[n];
        for (int i = 0; i < n; i++)
        {
            labels[i] = ButtonView("Label " + String(i));
            body.add(labels[i]);
        }

        int n1 = 10;
        footer.setSpacing(5);

        add(header);
        add(body);
        add(footer);

        body.selectEvent += [](Container *l) 
        {
           l->animationResize(Size(320, 200), 300);
        };
        body.unselectEvent += [](Container *l) 
        {
            l->animationResize(Size(160, 120), 300);
        };
    }

    bool onExit()
    {
        return true;
    }
};