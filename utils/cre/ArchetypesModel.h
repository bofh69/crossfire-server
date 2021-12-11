#ifndef ARCHETYPESMODEL_H
#define ARCHETYPESMODEL_H

extern "C" {
#include "global.h"
#include "object.h"
}

#include <set>
#include <QAbstractTableModel>

class ResourcesManager;
class ObjectWrapper;

class ArchetypesModel : public QAbstractTableModel {
  Q_OBJECT
public:
  ArchetypesModel(ResourcesManager *resourcesManager);
  virtual ~ArchetypesModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

signals:
  void archetypeModified(archetype *arch);

private:
  std::vector<ObjectWrapper *> myMonsters;
};

#endif /* ARCHETYPESMODEL_H */

