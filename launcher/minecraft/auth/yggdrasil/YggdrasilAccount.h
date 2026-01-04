//
// Created by HowXu on 2025/12/26.
//


#pragma once

#include "minecraft/auth/BaseAccount.h"

class YggdrasilAccount;

using YggdrasilAccountPtr = shared_qobject_ptr<YggdrasilAccount>;
Q_DECLARE_METATYPE(YggdrasilAccountPtr)

class YggdrasilAccount : public BaseAccount {
    Q_OBJECT
   public: /*constructions*/
    explicit YggdrasilAccount(QObject* parent = 0);

public: /* static methods */
    static YggdrasilAccountPtr createYggdrasil(const QString& login, const QString& authUrl, const QString& loginUrl, const QString& refreshUrl);

public: /* methods */
    shared_qobject_ptr<AuthFlow> login(QString password);
};