#pragma once

#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "RichParameter.h"
#include "RichParameterSet.h"

/// @deprecated renamed to RichParameterWidget.h
class MeshLabWidget : public QWidget {}



//class Matrix44fWidget : public  MeshLabWidget
//{
//public:
//	Matrix44fWidget(QWidget* p,RichMatrix44f* rpar);
//
//	void collectWidgetValue();
//	void resetWidgetValue();
//};

/*
class FloatListWidget : public MeshLabWidget
{
public:
	FloatListWidget(QWidget* p,RichFloatList* rpar);

	void collectWidgetValue();
	void resetWidgetValue();
};
*/

/*
class OpenFileWidget : public MeshLabWidget
{
public:
	OpenFileWidget(QWidget* p,RichOpenFile* rpar);

	void collectWidgetValue();
	void resetWidgetValue();
};
*/


/// Widget to enter a color.
// public QHBoxLayout,
class ColorWidget : public MeshLabWidget {
    Q_OBJECT

    QPushButton* colorButton;
    QLabel* colorLabel;
    QLabel* descLabel;
    QColor pickcol;

public:
    ColorWidget(QWidget *p, RichColor* newColor);
    ~ColorWidget();

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);
    void initWidgetValue();

private:
    void  updateColorInfo(const ColorValue& newColor);

signals:
    void dialogParamChanged();
private slots:
    void pickColor();
};



/// Widget to enter a value as a percentage or as an absolute value.
/// You have to specify the default value and the range of the possible values.
/// The default value is expressed in ABSolute units (e.g. it should be in the min..max range.


//public QGridLayout
class AbsPercWidget : public MeshLabWidget {
    Q_OBJECT

public:
    AbsPercWidget(QWidget *p,RichAbsPerc* rabs);
    ~AbsPercWidget();

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

private:
    void  setValue(float val, float minV, float maxV);

public slots:

    void on_absSB_valueChanged(double newv);
    void on_percSB_valueChanged(double newv);
signals:
    void dialogParamChanged();

protected:
    QDoubleSpinBox *absSB;
    QDoubleSpinBox *percSB;
    QLabel* fieldDesc;
    float m_min;
    float m_max;
};


#ifdef OLDMESHLAB
/// Widget to enter a Point3f,
/// if at the creation you provide a pointer to a GLArea (the mesh viewing window)
/// the widget exposes a button for getting the current view directiont
class Point3fWidget : public MeshLabWidget {
    Q_OBJECT

public:
    Point3fWidget(QWidget *p, RichPoint3f* rpf, QWidget *gla);
    ~Point3fWidget();
    QString paramName;
    vcg::Point3f getValue();

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

public slots:
    void  getPoint();
    void  setValue(QString name, vcg::Point3f val);
    void  setShotValue(QString name, vcg::Shotf val);
signals:
    void askViewDir(QString);
    void askViewPos(QString);
    void askSurfacePos(QString);
    void askCameraPos(QString);

protected:
    QLineEdit * coordSB[3];
    QComboBox *getPoint3Combo;
    QPushButton *getPoint3Button;
    QLabel* descLab;
};

class Matrix44fWidget : public MeshLabWidget {
    Q_OBJECT

public:
    Matrix44fWidget(QWidget *p,  RichMatrix44f* rpf, QWidget *gla_curr);
    ~Matrix44fWidget();
    QString paramName;
    vcg::Matrix44f getValue();

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

public slots:
    void  setValue(QString name, vcg::Matrix44f val);
    void getMatrix();
    void pasteMatrix();
signals:
    void askMeshMatrix(QString);


protected:
    QLineEdit * coordSB[16];
    QPushButton *getPoint3Button;
    QLabel* descLab;
};

/// Widget to enter a Shot,
/// if at the creation you provide a pointer to a GLArea (the mesh viewing window)
/// the widget has a combo and a button to get shot from different places.

class ShotfWidget : public MeshLabWidget {
    Q_OBJECT

public:
    ShotfWidget(QWidget *p, RichShotf* rpf, QWidget *gla);
    ~ShotfWidget();
    QString paramName;
    vcg::Shotf getValue();

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

public slots:
    void  getShot();
    void  setShotValue(QString name, vcg::Shotf val);
signals:
    void askRasterShot(QString);
    void askMeshShot(QString);
    void askViewerShot(QString);

protected:
    vcg::Shotf curShot;
    QLineEdit * shotLE;
    QPushButton *getShotButton;
    QComboBox *getShotCombo;
    QLabel* descLab;
};


//public QGridLayout,
class DynamicFloatWidget : public MeshLabWidget {
    Q_OBJECT

public:
    DynamicFloatWidget(QWidget *p, RichDynamicFloat* rdf);
    ~DynamicFloatWidget();

    float getValue();
    void  setValue(float val, float minV, float maxV);

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

public slots:
    void setValue(int newv);
    void setValue();
    void setValue(float newValue);

signals:
    //void valueChanged(int mask);
    void dialogParamChanged();

protected:
    QLineEdit *valueLE;
    QSlider   *valueSlider;
    QLabel* fieldDesc;
    float minVal;
    float maxVal;
private :
    float intToFloat(int val);
    int floatToInt(float val);
};

#endif

class ComboWidget : public MeshLabWidget {
    Q_OBJECT
protected:
    QComboBox *enumCombo;
    QLabel *enumLabel;
public:
    ComboWidget(QWidget *p, RichParameter* rpar);
    ~ComboWidget();
    void Init(QWidget *p,int newEnum, QStringList values);
    virtual void collectWidgetValue() = 0;
    virtual void resetWidgetValue() = 0;
    virtual void setWidgetValue(const Value& nv) = 0;

    int getIndex();
    void  setIndex(int newEnum);

signals:
    void dialogParamChanged();
};

/// Widget to select an entry from a list

//public QHBoxLayout
class EnumWidget : public ComboWidget {
    Q_OBJECT

public:
    EnumWidget(QWidget *p, RichEnum* rpar);
    ~EnumWidget() {};

    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

    //returns the number of items in the list
    int getSize();
};

class IOFileWidget : public MeshLabWidget {
    Q_OBJECT

protected:
    IOFileWidget(QWidget* p,RichParameter* rpar);
    ~IOFileWidget();

    void  updateFileName(const FileValue& file);

public:
    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

protected slots:
    virtual void selectFile() = 0;

signals:
    void dialogParamChanged();


protected:
    QLineEdit* filename;
    QPushButton* browse;
    QString fl;
    QLabel* descLab;
};

class RichParameterSetWidget : public MeshLabWidget{
    Q_OBJECT
public:
    RichParameterSetWidget(QWidget* p,RichParameterSet* rpar) 
        : MeshLabWidget(p, rpar) {
        /// @todo write recursive...
        assert(0);
    }
    ~RichParameterSetWidget(){}
    /// @todo to be completed
    virtual void resetWidgetValue(){assert(0);}
    /// @todo to be completed
    virtual void collectWidgetValue(){assert(0);}
    /// @todo to be completed
    virtual void setWidgetValue(const Value& nv){assert(0);}
};

#ifdef OLDMESHLAB
class SaveFileWidget : public IOFileWidget {
    Q_OBJECT
public:
    SaveFileWidget(QWidget* p,RichSaveFile* rpar);
    ~SaveFileWidget();

protected slots:
    void selectFile();

};

class OpenFileWidget : public IOFileWidget {
    Q_OBJECT
public:
    OpenFileWidget(QWidget *p, RichOpenFile* rdf);
    ~OpenFileWidget();

protected slots:
    void selectFile();
};
#endif

/*
This class is used to automatically create a frame from a set of parameters.
it is used mostly for creating the main dialog of the filters, but it is used also
in the creation of the additional saving options, post and pre opening processing
and for general parameter setting in edit plugins (e.g. look at the aligment parameters)
*/
class StdParFrame : public QFrame {
    Q_OBJECT
public:
    StdParFrame(QWidget *p, QWidget *gla=0);

    void loadFrameContent(RichParameterSet &curParSet, Document *mdPt = 0);
    void loadFrameContent(RichParameter* par, Document *mdPt = 0);

    // The curParSet that is passed must be 'compatible' with the RichParameterSet that have been used to create the frame.
    // This function updates the RichParameterSet used to create the frame AND fill also the passed <curParSet>
    void readValues(RichParameterSet &curParSet);
    void resetValues(RichParameterSet &curParSet);

    void toggleHelp();

    QVector<MeshLabWidget *> stdfieldwidgets;
    QVector<QLabel *> helpList;

    QWidget *gla; // used for having a link to the glarea that spawned the parameter asking.
    ~StdParFrame();
signals:

    void dynamicFloatChanged(int mask);
    void parameterChanged();
};



// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structred form and having some integrated help
class GenericParamDialog: public QDialog {
    Q_OBJECT
public:
    GenericParamDialog(QWidget *p, RichParameterSet *_curParSet, QString title=QString(), Document* _meshDocument = 0);
    ~GenericParamDialog();

    RichParameterSet *curParSet;
    StdParFrame *stdParFrame;

    void createFrame();

public slots:
    void getAccept();
    void toggleHelp();

    //reset the values on the gui back to the ones originally given to the dialog
    void resetValues();

private:
    Document* meshDocument;
};


// This is the dialog used to ask parameters for the MeshLab filters.
// This dialog is automatically configurated starting from the parameter asked by a given filter.
// It can handle dynamic parameters that modify only partially a given mesh.
class MainWindow;
class MeshlabStdDialog : public QDockWidget {
    Q_OBJECT

public:
    MeshlabStdDialog(QWidget *p = NULL);
    ~MeshlabStdDialog();

    void clearValues();
    void createFrame();
    void loadFrameContent(Document *mdPt=0);

    bool showAutoDialog(FilterPlugin *mfi, Model *mm, Document* md, QAction *q, MainWindow* mwi, QWidget *gla=0);
    bool isDynamic();

public slots:
    void closeClick();
private slots:
    void applyClick();
    void resetValues();
    void toggleHelp();
    void togglePreview();
    void applyDynamic();

public:
    QFrame *qf;
    StdParFrame *stdParFrame;
    QAction *curAction;
    QCheckBox *previewCB;

    void closeEvent ( QCloseEvent * event );

    int curmask;
    Model *curModel;
    Document * curMeshDoc;
    /// The filter which execution caused invocation of the current dialog
    FilterPlugin* curmfi;
    MainWindow* curmwi;
    QWidget * curgla;
    RichParameterSet curParSet;
    RichParameterSet prevParSet;
    bool validcache;
};

//QWidget* parent parameter says to the class who will destroy the MeshLabWidget object that it had created
//RichWidgetConstructor shouldn't destroy anything



class RichParameterToQTableWidgetItemConstructor : public Visitor {
public:
    RichParameterToQTableWidgetItemConstructor(/*QListWidget* widlst*/):/*lst(widlst),*/lastCreated(NULL) {}

    void visit(RichBool& pd);
    void visit(RichInt& pd);
    void visit(RichFloat& pd);
    void visit(RichString& pd);
    void visit(RichColor& pd);
    void visit(RichAbsPerc& pd);
    void visit(RichEnum& pd);
#ifdef OLDMESHLAB       
    void visit(RichMatrix44f& /*pd*/) { assert(0); };
    void visit(RichPoint3f& pd);
    void visit(RichShotf& pd);
    void visit(RichDynamicFloat& pd);
    void visit(RichOpenFile& pd);
    void visit(RichFloatList& /*pd*/) { assert(0); };
    void visit(RichSaveFile& /*pd*/) { assert(0); };
#endif

    QTableWidgetItem* lastCreated;
};
