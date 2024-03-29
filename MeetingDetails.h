#ifndef MEETINGDETAILS_H
#define MEETINGDETAILS_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "Commons.h"
#include "CheckBoxList.h"

class MeetingDetails : public QDialog
{
Q_OBJECT
public:
    MeetingDetails(int _user_id, int _meeting_id, QWidget *parent = 0);

private:
    int user_id;
    int meeting_id;
    
    QLabel *lb_label;
    QLabel *lb_begin;
    QLabel *lb_end;

    QTimeEdit *te_duration;
    QPushButton *btn_editDuration;

    QComboBox *cb_organizer;

    QComboBox *cb_users;

    QLabel *lb_room;
    QComboBox *cb_equipments;

    QPushButton *btn_unavailable;
    QPushButton *btn_del;
    QPushButton *btn_action;
    QPushButton *btn_cancel;

private slots:
    void canceledMeeting();
    void deleteMeeting();
    void editDuration();

signals:
    void notifyRefreshList();

};

#endif // MeetingActions_H
