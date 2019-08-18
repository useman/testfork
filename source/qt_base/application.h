//
// Aspia Project
// Copyright (C) 2019 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef QT_BASE__APPLICATION_H
#define QT_BASE__APPLICATION_H

#include "base/threading/thread.h"
#include "qt_base/locale_loader.h"

#include <asio/io_context.hpp>

#include <QApplication>

class QLocalServer;
class QLockFile;

namespace crypto {
class ScopedCryptoInitializer;
} // namespace crypto

namespace qt_base {

class Application
    : public QApplication,
      public base::Thread::Delegate
{
    Q_OBJECT

public:
    Application(int& argc, char* argv[]);
    virtual ~Application();

    static Application* instance();

    static std::shared_ptr<base::MessageLoopProxy> ioRunner();
    static asio::io_context* ioContext();

    bool isRunning();

    using Locale = LocaleLoader::Locale;
    using LocaleList = LocaleLoader::LocaleList;

    LocaleList localeList() const;
    void setLocale(const QString& locale);
    bool hasLocale(const QString& locale);

public slots:
    void sendMessage(const QByteArray& message);

signals:
    void messageReceived(const QByteArray& message);

protected:
    // base::Thread::Delegate implementation.
    void onBeforeThreadRunning() override;

private slots:
    void onNewConnection();

private:
    QString lock_file_name_;
    QString server_name_;

    QLockFile* lock_file_ = nullptr;
    QLocalServer* server_ = nullptr;

    std::unique_ptr<crypto::ScopedCryptoInitializer> crypto_initializer_;
    std::unique_ptr<LocaleLoader> locale_loader_;

    base::Thread io_thread_;
    std::shared_ptr<base::MessageLoopProxy> io_runner_;

    DISALLOW_COPY_AND_ASSIGN(Application);
};

} // namespace qt_base

#endif // QT_BASE__APPLICATION_H