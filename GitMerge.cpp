#include "GitMerge.h"

#include <GitBase.h>
#include <GitWip.h>
#include <QLogger.h>

#include <QFile>

using namespace QLogger;

GitMerge::GitMerge(const QSharedPointer<GitBase> &gitBase)
   : mGitBase(gitBase)
{
}

bool GitMerge::isInMerge() const
{
   QFile mergeHead(QString("%1/MERGE_HEAD").arg(mGitBase->getGitDir()));

   return mergeHead.exists();
}

GitExecResult GitMerge::merge(const QString &into, QStringList sources)
{
   QLog_Debug("Git", QString("Executing merge: {%1} into {%2}").arg(sources.join(","), into));

   {
      const auto cmd = QString("git checkout -q %1").arg(into);

      QLog_Trace("Git", QString("Checking out the current branch: {%1}").arg(cmd));

      const auto retCheckout = mGitBase->run(cmd);

      if (!retCheckout.success)
         return retCheckout;
   }

   const auto cmd2 = QString("git merge -Xignore-all-space ").append(sources.join(" "));

   QLog_Trace("Git", QString("Merging ignoring spaces: {%1}").arg(cmd2));

   return mGitBase->run(cmd2);
}

GitExecResult GitMerge::abortMerge() const
{
   QLog_Debug("Git", QString("Aborting merge"));

   const auto cmd = QString("git merge --abort");

   QLog_Trace("Git", QString("Aborting merge: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitMerge::applyMerge(const QString &msg) const
{
   QLog_Debug("Git", QString("Committing merge"));

   const auto cmd = QString("git commit %1")
                        .arg(msg.isEmpty() ? QString::fromUtf8("--no-edit") : QString::fromUtf8("-m \"%1\"").arg(msg));

   QLog_Trace("Git", QString("Committing merge: {%1}").arg(cmd));

   const auto ret = mGitBase->run(cmd);

   return ret;
}

GitExecResult GitMerge::squashMerge(const QString &into, QStringList sources, const QString &msg) const
{
   QLog_Debug("Git", QString("Executing squash merge: {%1} into {%2}").arg(sources.join(","), into));

   {
      const auto cmd = QString("git checkout -q %1").arg(into);

      QLog_Trace("Git", QString("Checking out the current branch: {%1}").arg(cmd));

      const auto retCheckout = mGitBase->run(cmd);

      if (!retCheckout.success)
         return retCheckout;
   }

   const auto cmd2 = QString("git merge  -Xignore-all-space --squash ").append(sources.join(" "));

   const auto retMerge = mGitBase->run(cmd2);

   if (retMerge.success)
   {
      if (msg.isEmpty())
      {
         const auto commitCmd = QString("git commit --no-edit");
         mGitBase->run(commitCmd);
      }
      else
      {
         const auto cmd = QString("git commit -m \"%1\"").arg(msg);
         mGitBase->run(cmd);
      }
   }

   return retMerge;
}

GitExecResult GitMerge::rebase(const QString &ontoBranch) const
{
   QLog_Debug("Git", QString("Executing rebase: {%1} onto {%2}").arg(ontoBranch, mGitBase->getCurrentBranch()));

   const auto cmd = QString("git rebase %1").arg(ontoBranch);

   return mGitBase->run(cmd);
}

GitExecResult GitMerge::rebaseAbort() const
{
   QLog_Debug("Git", QString("Aborting rebase"));

   const auto cmd = QString("git rebase --abort");

   return mGitBase->run(cmd);
}
