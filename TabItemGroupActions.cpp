#include "TabItemGroupActions.h"

TabItemGroupActions::TabItemGroupActions(QWidget *parent) : QDialog(parent)
{
    le_group_name = new QLineEdit();

    QFormLayout *fl_data = new QFormLayout;
    fl_data->addRow("Nom:", le_group_name);

    btn_cancel = new QPushButton("&Annuler");
    btn_action = new QPushButton();


    setWindowTitle("Cr�er un groupe");
    btn_action->setText("&Ajouter !");


    QHBoxLayout *layout_buttons = new QHBoxLayout;
    layout_buttons->setAlignment(Qt::AlignRight);
    layout_buttons->addWidget(btn_action);
    layout_buttons->addWidget(btn_cancel);

    QVBoxLayout *layout_main = new QVBoxLayout;
    layout_main->addLayout(fl_data);
    layout_main->addLayout(layout_buttons);

    setLayout(layout_main);

    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_action, SIGNAL(clicked()), this, SLOT(makeAction()));
}

void TabItemGroupActions::makeAction(){
    QString missingFields("");
    if(le_group_name->text() == "") missingFields += "Nom ; ";

    if (missingFields == ""){ // Si tout a �t� saisi

        QSqlQuery *req = new QSqlQuery;

        req->prepare("SELECT grp_name FROM grp WHERE grp_name = :name");
        req->bindValue(":name", le_group_name->text());
        req->exec();

        // si il n'y a pas d'entr�e similaire
        if(!req->next()){
            req->prepare("INSERT INTO grp(grp_name) VALUES (:name)");
            req->bindValue(":name", le_group_name->text());

            if(req->exec()){
                emit notifyRefreshList();
                QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "Le groupe a �t� ajout� dans la base de donn�es !");
                accept();
            }
            else{
                QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
            }
        }
        else{
            QMessageBox::warning(this, "Erreur !", "Le groupe existe d�j� dans la base de donn�es !");
        }
    }
    else{
        QMessageBox::warning(this, "Action Impossible", "Veuillez remplir les champs vides :\n"+missingFields);
    }
}
