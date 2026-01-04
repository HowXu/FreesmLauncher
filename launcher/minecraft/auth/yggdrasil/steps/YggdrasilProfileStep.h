//
// Created by HowXu on 2026/1/4.
//
#pragma once

#include <QObject>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Download.h"
#include "net/RawHeaderProxy.h"

class YggdrasilProfileStep : public AuthStep {
    Q_OBJECT

   public:
    explicit YggdrasilProfileStep(AccountData* data);
    virtual ~YggdrasilProfileStep() noexcept = default;

    void perform() override;
    QString describe() override;

private slots:
 void onRequestDone();

private:
    QUrl getSessionServerEndpoint(const QString& endpoint) const;

    std::shared_ptr<QByteArray> m_response;
    Net::Download::Ptr m_request;
    NetJob::Ptr m_task;
};