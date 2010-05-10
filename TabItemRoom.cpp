#include "TabItemRoom.h"

TabItemRoom::TabItemRoom(QWidget *parent) : QWidget(parent)
{
    model = new QSqlQueryModel();

    view = new QTableView();
    view->setDragEnabled(true);
    view->verticalHeader()->hide();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setAlternatingRowColors(true);
    view->setSortingEnabled (true);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    proxyModel = new QSortFilterProxyModel();

    refreshList();

    QVBoxLayout *layout_main = new QVBoxLayout();
    layout_main->addWidget(view);

    setLayout(layout_main);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createMenu(QPoint)));

}

void TabItemRoom::refreshList(){
    model->setQuery("SELECT * FROM room ORDER BY name");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Capacit�");

    proxyModel->setSourceModel(model);

    view->setModel(proxyModel);
    view->hideColumn(0);
}

void TabItemRoom::createMenu(QPoint pos){
    QMenu *menu = new QMenu(this);
    QAction *action_add = new QAction("&Nouveau", this);
    menu->addAction(action_add);

    QModelIndex index = view->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){

        QAction *action_del = new QAction("&Supprimer", this);
        QAction *action_edit = new QAction("&Modifier", this);
        menu->addAction(action_del);
        menu->addAction(action_edit);

        connect(action_del, SIGNAL(triggered()), this, SLOT(deleteItem()));
        connect(action_edit, SIGNAL(triggered()), this, SLOT(editItem()));
    }
    connect(action_add, SIGNAL(triggered()), this, SLOT(addItem()));
    menu->exec(view->mapToGlobal(pos));
}

void TabItemRoom::deleteItem(){
    QModelIndex index = view->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int id_room = proxyModel->data(index).toInt();
        QSqlQuery *req = new QSqlQuery();
        req->prepare("DELETE FROM room WHERE id = :id_room");
        req->bindValue("id_room", id_room);

        int rep = QMessageBox::question(this, "Confirmation", "Etes-vous s�r de vouloir supprimer cette salle ?", QMessageBox::Yes | QMessageBox::No);
        if(rep == QMessageBox::Yes){

            if (req->exec()){
                refreshList();
                QMessageBox::information(this, "Requ�te ex�cut�e avec succ�s !", "La salle a �t� supprim� de la base de donn�es !");
            }
            else
                QMessageBox::warning(this, "Erreur !", "La requ�te n'a pas pu �tre ex�cut�e ! \nDes r�unions peut-�tre rattach�es � cette salle ...");
        }
    }
}

void TabItemRoom::editItem(){
    QModelIndex index = view->selectionModel()->currentIndex();
    index = index.sibling(index.row(), 0); // ici je force le n� de colonne � 0, pour etre sur le premier champ
    if (index.row() != -1){
        int id_room = proxyModel->data(index).toInt();

        TabItemRoomActions *roomEdit = new TabItemRoomActions(EDIT, this, id_room);
        connect(roomEdit, SIGNAL(notifyRefreshList()), this, SLOT(refreshList()));
        roomEdit->exec();
    }
}

void TabItemRoom::addItem(){
    TabItemRoomActions *roomAdd = new TabItemRoomActions(ADD, this);
    connect(roomAdd, SIGNAL(notifyRefreshList()), this, SLOT(refreshList()));
    roomAdd->exec();
}
