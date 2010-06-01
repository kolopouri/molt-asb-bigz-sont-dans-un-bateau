#include "MeetingDetails.h"

MeetingDetails::MeetingDetails(int _id, QWidget *parent) : QDialog(parent)
{
    id = _id;

    setWindowTitle("D�tails de la r�union");

    QSqlQuery *req = new QSqlQuery();
    req->prepare("SELECT * FROM meeting m WHERE m.meeting_id = :meeting_id");
    req->bindValue(":meeting_id", id);
    req->exec();
    QSqlRecord rec = req->record();
    req->next();

    lb_label = new QLabel(req->value(rec.indexOf("meeting_label")).toString());
    lb_begin = new QLabel(QDateTime::fromString(req->value(rec.indexOf("meeting_begin")).toString(), "yyyy-MM-dd hh:mm").toString("dd/MM/yyyy hh:mm"));
    lb_end = new QLabel(QDateTime::fromString(req->value(rec.indexOf("meeting_end")).toString(), "yyyy-MM-dd hh:mm").toString("dd/MM/yyyy hh:mm"));

    cb_users = new QComboBox();

    req->prepare("SELECT * FROM meeting m INNER JOIN havemeeting hm ON m.meeting_id = hm.meeting_id INNER JOIN user u ON u.user_id = hm.user_id WHERE m.meeting_id = :meeting_id ORDER BY user_surname, user_name");
    req->bindValue(":meeting_id", id);
    req->exec();
    rec = req->record();
    while(req->next()){
        cb_users->addItem(req->value(rec.indexOf("user_surname")).toString() + " " + req->value(rec.indexOf("user_surname")).toString(), req->value(rec.indexOf("user_id")).toInt());
    }

    cb_equipments = new QComboBox();
    req->prepare("SELECT * FROM meeting m INNER JOIN room r ON r.room_id = m.room_id INNER JOIN haveequipment he ON he.room_id = r.room_id INNER JOIN equipment e ON he.equip_id = e.equip_id WHERE m.meeting_id = :meeting_id ORDER BY equip_name");
    req->bindValue(":meeting_id", id);
    req->exec();
    rec = req->record();
    while(req->next()){
        cb_equipments->addItem(req->value(rec.indexOf("equip_name")).toString(), req->value(rec.indexOf("equip_id")).toInt());
    }

    lb_room = new QLabel();
    req->first();
    lb_room->setText(req->value(rec.indexOf("room_name")).toString());

    QFormLayout *fl_data = new QFormLayout;
    fl_data->addRow("Libell�:", lb_label);
    fl_data->addRow("D�but:", lb_begin);
    fl_data->addRow("Fin:", lb_end);
    fl_data->addRow("Invit�(s):", cb_users);
    fl_data->addRow("Salle:", lb_room);
    fl_data->addRow("Equipement(s):", cb_equipments);

    btn_unavailable = new QPushButton("Ne sera pas pr�sent");
    btn_action = new QPushButton("Ok");
    btn_cancel = new QPushButton("&Annuler");


    QHBoxLayout *layout_buttons = new QHBoxLayout;
    layout_buttons->setAlignment(Qt::AlignRight);
    layout_buttons->addWidget(btn_unavailable);
    layout_buttons->addWidget(btn_action);
    layout_buttons->addWidget(btn_cancel);

    QVBoxLayout *layout_main = new QVBoxLayout;
    layout_main->addLayout(fl_data);
    layout_main->addLayout(layout_buttons);

    setLayout(layout_main);

    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_action, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btn_unavailable, SIGNAL(clicked()), this, SLOT(canceledMeeting()));

}

void MeetingDetails::canceledMeeting(){
    emit notifyRefreshList();
}
