#include "TabItemNews.h"

TabItemNews::TabItemNews(int _user_id, QWidget *parent) : QWidget(parent)
{
    user_id = _user_id;

    lb_message_event = new QLabel();

    table = new QTableWidget(0,8,this);

    table->verticalHeader()->hide();
    table->hideColumn(0); // On cache la cl�
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    refreshListEvent();

    lb_message_mail = new QLabel("Bo�te mails");

    view_in = new QTableView();
    view_in->setDragEnabled(true);
    view_in->verticalHeader()->hide();
    view_in->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_in->setSelectionMode(QAbstractItemView::SingleSelection);
    view_in->setAlternatingRowColors(true);
    view_in->setSortingEnabled (true);
    view_in->setContextMenuPolicy(Qt::CustomContextMenu);

    view_out = new QTableView();
    view_out->setDragEnabled(true);
    view_out->verticalHeader()->hide();
    view_out->setSelectionBehavior(QAbstractItemView::SelectRows);
    view_out->setSelectionMode(QAbstractItemView::SingleSelection);
    view_out->setAlternatingRowColors(true);
    view_out->setSortingEnabled (true);
    view_out->setContextMenuPolicy(Qt::CustomContextMenu);

    proxyModel_in = new QSortFilterProxyModel();
    proxyModel_out = new QSortFilterProxyModel();


    btn_add = new QPushButton("Nouveau");
    btn_add->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btn_add2 = new QPushButton("Nouveau");
    btn_reply = new QPushButton("R�pondre");
    btn_reply->setDisabled(true);
    btn_del = new QPushButton("Supprimer");
    btn_del->setDisabled(true);
    btn_del2 = new QPushButton("Supprimer");
    btn_del2->setDisabled(true);

    QHBoxLayout *layout_btn2 = new QHBoxLayout();
    layout_btn2->addWidget(btn_add2);
    layout_btn2->addWidget(btn_reply);
    layout_btn2->addWidget(btn_del2);
    layout_btn2->addStretch();

    QHBoxLayout *layout_btn = new QHBoxLayout();
    layout_btn->addWidget(btn_add);
    layout_btn->addWidget(btn_del);
    layout_btn->addStretch();

    QWidget *tab_inbox = new QWidget();

    QVBoxLayout *layout_inbox = new QVBoxLayout();
    layout_inbox->addLayout(layout_btn2);
    layout_inbox->addWidget(view_in);

    tab_inbox->setLayout(layout_inbox);

    QWidget *tab_sent = new QWidget();

    QVBoxLayout *layout_sent = new QVBoxLayout();
    layout_sent->addLayout(layout_btn);
    layout_sent->addWidget(view_out);

    tab_sent->setLayout(layout_sent);

    tab_mail = new QTabWidget();
    tab_mail->addTab(tab_inbox, "Bo�te de reception");
    tab_mail->addTab(tab_sent, "Messages envoy�s");

    model_in = new QSqlQueryModel();
    model_out = new QSqlQueryModel();

    refreshListMail();

    QVBoxLayout *layout_main = new QVBoxLayout();
    layout_main->addWidget(lb_message_event);
    layout_main->addWidget(table);
    layout_main->addWidget(lb_message_mail);
    layout_main->addWidget(tab_mail);

    setLayout(layout_main);

    connect(view_in, SIGNAL(clicked(QModelIndex)), this, SLOT(refreshButtonState_in(QModelIndex)));
    connect(tab_mail, SIGNAL(currentChanged(int)), this, SLOT(refreshListMail()));
    connect(view_out, SIGNAL(clicked(QModelIndex)), this, SLOT(refreshButtonState_out(QModelIndex)));
    connect(btn_reply, SIGNAL(clicked()), this, SLOT(replyMail()));
    connect(btn_add, SIGNAL(clicked()), this, SLOT(sendMail()));
    connect(btn_add2, SIGNAL(clicked()), this, SLOT(sendMail()));
    connect(btn_del, SIGNAL(clicked()), this, SLOT(deleteItem_out()));
    connect(btn_del2, SIGNAL(clicked()), this, SLOT(deleteItem_in()));
    connect(view_in, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(displayMail_in()));
    connect(view_out, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(displayMail_out()));
}

void TabItemNews::refreshListEvent(){
    table->clear();
    table->setRowCount(0);

    QStringList title;
    title << "id_meeting" << "Libell�" << "D�but" << "Fin" << "P�riodicit�" << "Organisateur" << "Accepter" << "Refuser";
    table->setHorizontalHeaderLabels(title);

    int i=0, j;
    QSqlQuery *req = new QSqlQuery();
    QSqlRecord rec;
    req->prepare("SELECT * FROM meeting m INNER JOIN havemeeting hm ON hm.user_id = hm.user_id INNER JOIN user u ON u.user_id = m.user_id WHERE hm.user_id = :user_id AND hm.hm_state = :state ORDER BY meeting_begin DESC");
    req->bindValue(":user_id", user_id);
    req->bindValue(":state", MAY_ATTEND);
    req->exec();
    rec = req->record();

    QSignalMapper *mapper_accept = new QSignalMapper();
    QSignalMapper *mapper_decline = new QSignalMapper();

    while(req->next()){
        int id_meeting = req->value(rec.indexOf("meeting_id")).toInt();
        QString label = req->value(rec.indexOf("meeting_label")).toString();
        QString begin = QDateTime::fromString(req->value(rec.indexOf("meeting_begin")).toString(), "yyyy-MM-dd hh:mm").toString("dd/MM/yyyy hh:mm");
        QString end = QDateTime::fromString(req->value(rec.indexOf("meeting_end")).toString(), "yyyy-MM-dd hh:mm").toString("dd/MM/yyyy hh:mm");
        QString periodic;
        if(req->value(rec.indexOf("meeting_periodic")).toInt() == 0){
            periodic = "Ponctuel";
        }
        else if(req->value(rec.indexOf("meeting_periodic")).toInt() == 1){
            periodic = "Hebdomadaire";
        }
        else{
            periodic = "Mensuel";
        }

        QString organizer = req->value(rec.indexOf("user_surname")).toString() + " " + req->value(rec.indexOf("user_name")).toString();

        QPushButton *btn_accept = new QPushButton("Participera");
        QPushButton *btn_decline = new QPushButton("Ne participera pas");

        connect(btn_accept, SIGNAL(clicked()), mapper_accept, SLOT(map()));
        mapper_accept->setMapping(btn_accept, id_meeting);

        connect(btn_decline, SIGNAL(clicked()), mapper_decline, SLOT(map()));
        mapper_decline->setMapping(btn_decline, id_meeting);

        QTableWidgetItem *twi_id_meeting = new QTableWidgetItem(id_meeting);
        QTableWidgetItem *twi_label = new QTableWidgetItem(label);
        QTableWidgetItem *twi_begin = new QTableWidgetItem(begin);
        QTableWidgetItem *twi_end = new QTableWidgetItem(end);
        QTableWidgetItem *twi_periodic = new QTableWidgetItem(periodic);
        QTableWidgetItem *twi_organizer = new QTableWidgetItem(organizer);

        j = 0;
        table->insertRow(i);

        table->setItem(i, j, twi_id_meeting);
        table->setItem(i, j+1, twi_label);
        table->setItem(i, j+2, twi_begin);
        table->setItem(i, j+3, twi_end);
        table->setItem(i, j+4, twi_periodic);
        table->setItem(i, j+5, twi_organizer);
        table->setCellWidget(i, j+6, btn_accept);
        table->setCellWidget(i, j+7, btn_decline);

        i++;
    }

    connect(mapper_accept, SIGNAL(mapped(int)), this, SLOT(acceptMeeting(int)));
    connect(mapper_decline, SIGNAL(mapped(int)), this, SLOT(declineMeeting(int)));

    if(i == 0)
        lb_message_event->setText("Pas de nouveaux �venements.");
    else
        lb_message_event->setText("Vous avez des notifications � traiter.");
}

void TabItemNews::acceptMeeting(int id_meeting){

}

void TabItemNews::declineMeeting(int id_meeting){

}

void TabItemNews::refreshListMail(){

    QSqlQuery req;
    req.prepare("SELECT m.user_id_from, m.user_id_to, m.msg_from, u.user_surname, u.user_name, m.msg_date, m.msg_subject FROM message m INNER JOIN user u ON u.user_id = m.user_id_from WHERE user_id_to = :user_id AND m.msg_from = '0' ORDER BY msg_date DESC");
    req.bindValue(":user_id", user_id);
    req.exec();

    model_in->setQuery(req);
    model_in->setHeaderData(3, Qt::Horizontal, "Nom");
    model_in->setHeaderData(4, Qt::Horizontal, "Pr�nom");
    model_in->setHeaderData(5, Qt::Horizontal, "Le");
    model_in->setHeaderData(6, Qt::Horizontal, "Objet");

    proxyModel_in->setSourceModel(model_in);

    view_in->setModel(proxyModel_in);
    view_in->hideColumn(0);
    view_in->hideColumn(1);
    view_in->hideColumn(2);
    view_in->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    req.prepare("SELECT m.user_id_from, m.user_id_to, m.msg_from, u.user_surname, u.user_name, m.msg_date, m.msg_subject FROM message m INNER JOIN user u ON u.user_id = m.user_id_to WHERE user_id_from = :user_id AND msg_from = '1' ORDER BY msg_date DESC");
    req.bindValue(":user_id", user_id);
    req.exec();
    model_out->setQuery(req);
    model_out->setHeaderData(3, Qt::Horizontal, "Nom");
    model_out->setHeaderData(4, Qt::Horizontal, "Pr�nom");
    model_out->setHeaderData(5, Qt::Horizontal, "Le");
    model_out->setHeaderData(6, Qt::Horizontal, "Objet");

    proxyModel_out->setSourceModel(model_out);

    view_out->setModel(proxyModel_out);
    view_out->hideColumn(0);
    view_out->hideColumn(1);
    view_out->hideColumn(2);
    view_in->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void TabItemNews::refreshButtonState_in(QModelIndex index){
    if(index.isValid())
    {
        btn_reply->setDisabled(false);
        btn_del2->setDisabled(false);
    }
    else
    {
        btn_reply->setDisabled(true);
        btn_del2->setDisabled(true);
    }
}

void TabItemNews::refreshButtonState_out(QModelIndex index){
    if(index.isValid())
    {
        btn_del->setDisabled(false);
    }
    else
    {
        btn_del->setDisabled(true);
    }
}

void TabItemNews::replyMail(){
    QModelIndex index = view_in->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0);
    if (index.row() != -1){
        int user_id_to = proxyModel_in->data(index).toInt();
        index = index.sibling(index.row(), 6);
        QString subject = proxyModel_in->data(index).toString();

        SendMail *mailAdd = new SendMail(user_id, user_id_to, subject, this);
        connect(mailAdd, SIGNAL(notifyRefreshList()), this, SLOT(refreshListMail()));
        mailAdd->exec();
    }
}

void TabItemNews::sendMail(){
    SendMail *mailAdd = new SendMail(user_id, -1, "", this);
    connect(mailAdd, SIGNAL(notifyRefreshList()), this, SLOT(refreshListMail()));
    mailAdd->exec();
}

void TabItemNews::deleteItem_in(){
    QModelIndex index = view_in->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int user_id_from = proxyModel_in->data(index).toInt();
        index = index.sibling(index.row(), 1);
        int user_id_to = proxyModel_in->data(index).toInt();
        index = index.sibling(index.row(), 5);
        QString date = proxyModel_in->data(index).toString();

        QSqlQuery *req = new QSqlQuery();
        req->prepare("DELETE FROM message WHERE user_id_from = :user_id_from AND user_id_to = :user_id_to AND msg_date = :date AND msg_from = '0'");
        req->bindValue(":user_id_from", user_id_from);
        req->bindValue(":user_id_to", user_id_to);
        req->bindValue(":date", date);

        int rep = QMessageBox::question(this, "Confirmation", "Etes-vous s�r de vouloir supprimer ce message ?", QMessageBox::Yes | QMessageBox::No);
        if(rep == QMessageBox::Yes){
            if (req->exec()){
                refreshListMail();
                QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "Le message a �t� supprim� !");
            }
            else
                QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
        }
    }
}



void TabItemNews::deleteItem_out(){
    QModelIndex index = view_out->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int user_id_from = proxyModel_out->data(index).toInt();
        index = index.sibling(index.row(), 1);
        int user_id_to = proxyModel_out->data(index).toInt();
        index = index.sibling(index.row(), 5);
        QString date = proxyModel_out->data(index).toString();

        QSqlQuery *req = new QSqlQuery();
        req->prepare("DELETE FROM message WHERE user_id_from = :user_id_from AND user_id_to = :user_id_to AND msg_date = :date AND msg_from = '1'");
        req->bindValue(":user_id_from", user_id_from);
        req->bindValue(":user_id_to", user_id_to);
        req->bindValue(":date", date);

        int rep = QMessageBox::question(this, "Confirmation", "Etes-vous s�r de vouloir supprimer ce message ?", QMessageBox::Yes | QMessageBox::No);
        if(rep == QMessageBox::Yes){
            if (req->exec()){
                refreshListMail();
                QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "Le message a �t� supprim� !");
            }
            else
                QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e !");
        }
    }
}

void TabItemNews::displayMail_in(){
    QModelIndex index = view_in->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int user_id_from = proxyModel_in->data(index).toInt();
        index = index.sibling(index.row(), 1);
        int user_id_to = proxyModel_in->data(index).toInt();
        index = index.sibling(index.row(), 5);
        QString date = proxyModel_in->data(index).toString();

        MailDetails *mail_details = new MailDetails(user_id_from, user_id_to, date, this);
        mail_details->exec();

    }
}

void TabItemNews::displayMail_out(){
    QModelIndex index = view_out->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int user_id_from = proxyModel_out->data(index).toInt();
        index = index.sibling(index.row(), 1);
        int user_id_to = proxyModel_out->data(index).toInt();
        index = index.sibling(index.row(), 5);
        QString date = proxyModel_out->data(index).toString();

        MailDetails *mail_details = new MailDetails(user_id_from, user_id_to, date, this);
        mail_details->exec();
    }
}