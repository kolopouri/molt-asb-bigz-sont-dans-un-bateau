#include "TabItemRoomActions.h"

TabItemRoomActions::TabItemRoomActions(int _action, QWidget *parent, int _id) : QDialog(parent)
{
    action = _action;
    id = _id;

    le_room_name = new QLineEdit();
    le_room_capacity = new QSpinBox();
    le_room_capacity->setMinimum(1);

    QFormLayout *fl_data = new QFormLayout;
    fl_data->addRow("Nom:", le_room_name);
    fl_data->addRow("Capacit�:", le_room_capacity);

    btn_cancel = new QPushButton("&Annuler");
    btn_action = new QPushButton();

    if(action == ADD){
        setWindowTitle("Ajouter une salle");
        btn_action->setText("&Ajouter !");
    }
    else if(action == EDIT){
        setWindowTitle("Modifier une salle");
        btn_action->setText("&Enregistrer !");

        QSqlQuery req;
        req.prepare("SELECT * FROM room WHERE id = :id_room");
        req.bindValue("id_room", id);
        req.exec();

        QSqlRecord rec = req.record();

        if(req.first()){
            le_room_name->setText(req.value(rec.indexOf("name")).toString());
            le_room_capacity->setValue(req.value(rec.indexOf("capacity")).toInt());
        }
    }


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

void TabItemRoomActions::makeAction(){
    QString missingFields("");
    if(le_room_name->text() == "") missingFields += "Nom ; ";

    if (missingFields == ""){ // Si tout a �t� saisi
        QSqlQuery *req = new QSqlQuery;
        req->prepare("SELECT id, name FROM room WHERE name = :name");
        req->bindValue(":name", le_room_name->text());
        req->exec();

        if(action == EDIT){
            // si il n'y a pas d'entr�e similaire, ou si on modifie notre propre occurence
            if(!req->next() || req->value(0).toInt() == id){

                req->prepare("UPDATE room SET name = :name, capacity = :capacity WHERE id = :id");
                req->bindValue(":id", id);
                req->bindValue(":name", le_room_name->text());
                req->bindValue(":capacity", (int)le_room_capacity->value());

                if(req->exec()){
                    emit notifyRefreshList();
                    QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "La salle a �t� modifi� dans la base de donn�es !");
                    accept();
                }
                else{
                    QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
                }
            }
            else{
                QMessageBox::warning(this, "Erreur !", "La salle existe d�j� dans la base de donn�es !");
            }
        }
        else if(action == ADD){
            // si il n'y a pas d'entr�e similaire
            if(!req->next()){
                req->prepare("INSERT INTO room(name, capacity) VALUES (:name, :capacity)");
                req->bindValue(":name", le_room_name->text());
                req->bindValue(":capacity", le_room_capacity->value());

                if(req->exec()){
                    emit notifyRefreshList();
                    QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "La salle a �t� ajout� dans la base de donn�es !");
                    accept();
                }
                else{
                    QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
                }
            }
            else{
                QMessageBox::warning(this, "Erreur !", "La salle existe d�j� dans la base de donn�es !");
            }
        }
    }
    else{
        QMessageBox::warning(this, "Action Impossible", "Veuillez remplir les champs vides :\n"+missingFields);
    }
}
