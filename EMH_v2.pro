QT += widgets
QT += sql
CONFIG += c++11

FORMS += \
    GUI/loginwindow.ui \
    GUI/aboutmydevs.ui

HEADERS += \
    Model/Entity/Constraint.hpp \
    Model/Entity/DefaultValue.hpp \
    Model/Entity/Entity.hpp \
    Model/Entity/Field.hpp \
    Model/Entity/Form.hpp \
    Model/Entity/Group.hpp \
    Model/Entity/Tag.hpp \
    Model/Entity/User.hpp \
    Model/Relation/Access.hpp \
    Model/Relation/Belong.hpp \
    Model/Relation/Categorizing.hpp \
    Model/Relation/Contains.hpp \
    Model/Relation/Read.hpp \
    Model/Relation/Relation.hpp \
    Model/Relation/Require.hpp \
    Model/Relation/Write.hpp \
    Model/Persistable.hpp \
    Utilty/PersisterManager.hpp \
    Utilty/UserHandle.hpp \
    GUI/loginwindow.hpp \
    GUI/aboutmydevs.h

SOURCES += \
    Model/Entity/Constraint.cpp \
    Model/Entity/DefaultValue.cpp \
    Model/Entity/Entity.cpp \
    Model/Entity/Field.cpp \
    Model/Entity/Form.cpp \
    Model/Entity/Group.cpp \
    Model/Entity/Tag.cpp \
    Model/Entity/User.cpp \
    Model/Relation/Access.cpp \
    Model/Relation/Belong.cpp \
    Model/Relation/Categorizing.cpp \
    Model/Relation/Contains.cpp \
    Model/Relation/Read.cpp \
    Model/Relation/Require.cpp \
    Model/Relation/Write.cpp \
    Model/Persistable.cpp \
    Utilty/PersisterManager.cpp \
    Utilty/UserHandle.cpp \
    GUI/loginwindow.cpp \
    main.cpp \
    GUI/aboutmydevs.cpp

RESOURCES += \
    Resource/images.qrc