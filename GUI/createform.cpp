#include "createform.hpp"
#include "ui_createform.h"


createForm::createForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createForm), m_color(Qt::white), m_nbField(0),
    m_form(), m_formIsValid(false)
{
    ui->setupUi(this);

    m_listReadersLayout = new QVBoxLayout();
    m_listWritersLayout = new QVBoxLayout();
    m_listTagsLayout = new QVBoxLayout();
    m_fieldsLayout = new QVBoxLayout();

    ui->scrollAreaReaders->setLayout(m_listReadersLayout);
    ui->scrollAreaWriters->setLayout(m_listWritersLayout);
    ui->scrollAreaTags->setLayout(m_listTagsLayout);
    ui->scrollAreaFields->setLayout(m_fieldsLayout);

    // On parcourt les groupes
    for (auto groupname : Entity::Group::getAll())
    {
        m_listReaders.append(new QCheckBox(groupname));
        m_listWriters.append(new QCheckBox(groupname));

        m_listReadersLayout->addWidget(m_listReaders.last());
        m_listWritersLayout->addWidget(m_listWriters.last());
    }

    // On parcourt les tags
    for (auto tagname : Entity::Tag::getAll())
    {
        m_listTags.append(new QCheckBox(tagname));

        m_listTagsLayout->addWidget(m_listTags.last());
    }
}

createForm::~createForm()
{
    delete ui;

    for (auto it = m_fieldsWindows.begin(); it != m_fieldsWindows.end(); ++it)
        delete *it;
}

void createForm::chooseColor()
{
    m_color = QColorDialog::getColor(m_color);
}

void createForm::addField()
{
    m_nbField++;

    bool ok = false;

    m_fieldsWindows.insert(m_nbField, new fieldWindow(&ok, this));
        m_fieldsWindows.last()->exec();

    if (ok)
    {
        m_lines.insert( m_nbField, new QHBoxLayout() );

        m_edits.insert( m_nbField, new CustomQPushButton("Modifier", m_nbField) );
        m_deletes.insert( m_nbField, new CustomQPushButton("Supprimer", m_nbField) );

        QObject::connect(m_edits.last(), SIGNAL(customClicked(int)), this, SLOT(editField(int)));
        QObject::connect(m_deletes.last(), SIGNAL(customClicked(int)), this, SLOT(deleteField(int)));

        m_lines.last()->addWidget( new QLabel(m_fieldsWindows.last()->getField().getTypeReadable()) );
        m_lines.last()->addWidget( new QLabel(m_fieldsWindows.last()->getField().getLabel()) );
        m_lines.last()->addWidget( new QLabel(QString("%1 contrainte(s)").arg(m_fieldsWindows.last()->getNbConstraint()) ) );
        m_lines.last()->addWidget( m_edits.last() );
        m_lines.last()->addWidget( m_deletes.last() );

        m_fieldsLayout->addLayout(m_lines.last());
    }
    else
    {
        delete m_fieldsWindows.take(m_nbField);
    }
}

void createForm::editField(int id)
{
    m_fieldsWindows[id]->exec();

    QLabel *labelType = dynamic_cast<QLabel*>(m_lines[id]->itemAt(0)->widget());
        labelType->setText( m_fieldsWindows[id]->getField().getTypeReadable() );

    QLabel *labelName = dynamic_cast<QLabel*>(m_lines[id]->itemAt(1)->widget());
        labelName->setText( m_fieldsWindows[id]->getField().getLabel() );

    QLabel *nbConstraints = dynamic_cast<QLabel*>(m_lines[id]->itemAt(2)->widget());
        nbConstraints->setText( QString("%1 contrainte(s)").arg(m_fieldsWindows[id]->getNbConstraint()) );
}

void createForm::deleteField(int id)
{
    // On deco les slots
    QObject::disconnect(m_edits[id], SIGNAL(customClicked(int)), this, SLOT(editField(int)));
    QObject::disconnect(m_deletes[id], SIGNAL(customClicked(int)), this, SLOT(deleteField(int)));

    // On supprime la fenêtre
    delete m_fieldsWindows.take(id);

    // On récupère le layout horizontal
    QHBoxLayout* line = m_lines.take(id);

    // Supprime le horizontal layout du vertial layout
    m_fieldsLayout->removeItem( line );

    QLayoutItem *item;

    while ((item = line->takeAt(0)) != 0)
    {
        item->widget()->deleteLater();
        delete item;
    }

    delete line;

    // TODO Stocker les fields supprimer pour call la méthode remove quand il s'agit d'un formulaire existant
}

void createForm::valid()
{
    m_form.setColor(m_color);
    m_form.setDescription(ui->description->toPlainText());
    m_form.setIdAuthor( dynamic_cast<Dashboard*> ( parent() )->getUserId() );
    m_form.setImportant(ui->messageImportant->toPlainText());
    m_form.setInfo(ui->messageInformation->toPlainText());
    m_form.setName(ui->nomLineEdit->text());    

    if (ui->demandeRadioButton->isChecked())
        m_form.setStatus(Entity::Form::Status::DEMAND);
    else if (ui->informationRadioButton->isChecked())
        m_form.setStatus(Entity::Form::Status::INFORMATION);

    validForm();

    if (m_formIsValid)
    {
        Utility::PersisterManager pm;

        pm.persistOne(m_form);

        // On enregistre ceux qui pourront utiliser le formulaire
        for (QCheckBox* writersBox : m_listWriters)
        {
            if (writersBox->isChecked())
            {
                Entity::Group group;
                    auto error = group.loadByName(writersBox->text());

                if (Entity::Entity::ErrorType::NONE == error)
                {
                    Relation::Write writer;
                        writer.setIdForm(m_form.getId());
                        writer.setIdGroup(group.getId());

                    pm.persistOne(writer);
                }
            }
        }

        // On enregistre ceux qui pourront lire les soumissions du formulaire
        for (QCheckBox* readersBox : m_listReaders)
        {
            if (readersBox->isChecked())
            {
                Entity::Group group;
                    auto error = group.loadByName(readersBox->text());

                if (Entity::Entity::ErrorType::NONE == error)
                {
                    Relation::Read reader;
                        reader.setIdForm(m_form.getId());
                        reader.setIdGroup(group.getId());

                    pm.persistOne(reader);
                }
            }
        }

        // Enregistre les tags liés au formulaire
        for (QCheckBox* tagBox : m_listTags)
        {
            if (tagBox->isChecked())
            {
                Entity::Tag tag;
                    auto error = tag.loadByName(tagBox->text());

                if (Entity::Entity::ErrorType::NONE == error)
                {
                    Relation::Categorizing categorizing;
                        categorizing.setIdForm(m_form.getId());
                        categorizing.setIdTag(tag.getId());

                    pm.persistOne(categorizing);
                }
            }
        }

        for (auto it = m_fieldsWindows.begin(); it != m_fieldsWindows.end(); ++it)
        {            
            (*it)->persistField(m_form.getId());            
        }

        close();
    }
}

void createForm::displayError(QString message)
{
    m_formIsValid = false;

    QMessageBox::critical(this, "Erreur", message);
}

void createForm::validForm()
{
    m_formIsValid = true;

    if (Entity::Form::formExist(m_form.getName()))
    {
        displayError("Le nom du formulaire est déjà existant.");
        return;
    }

    if (!m_form.isValid()) {
        displayError("Le formulaire n'est pas valide (incomplet).");
        return;
    }

    Utility::HasCheckBoxChecked hascheckboxcheck;

    int nb = 0;

    nb = std::count_if(m_listReaders.begin(), m_listReaders.end(), hascheckboxcheck);

    if (nb == 0)
    {
        displayError("Le formulaire doit avoir des lecteurs.");
        return;
    }

    nb = std::count_if(m_listWriters.begin(), m_listWriters.end(), hascheckboxcheck);

    if (nb == 0)
    {
        displayError("Le formulaire doit avoir des utilisateurs.");
        return;
    }

    bool valid = false;

    for (auto &fieldWindow : m_fieldsWindows)
    {
        QObject::connect(fieldWindow, SIGNAL(sendError(QString)), this, SLOT(displayError(QString)));
        valid = !fieldWindow->validField();
        QObject::disconnect(fieldWindow, SIGNAL(sendError(QString)), this, SLOT(displayError(QString)));

        if (!valid) {
            return;
        }
    }
}
