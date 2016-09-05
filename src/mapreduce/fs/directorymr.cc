#include "directorymr.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../../common/context.hh"

namespace eclipse {

void DirectoryMR::init_db() {
  open_db();
  // Create SQL statement of IData
  sprintf(sql, "CREATE TABLE idata_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      num_reducer    INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id));"); 
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("idata_table created successfully\n");
  }
  // Create SQL statement of IGroup
  sprintf(sql, "CREATE TABLE igroup_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      reducer_id     INT       NOT NULL, \
      num_block      INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id, reducer_id));"); 
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("igroup_table created successfully\n");
  }
  // Create SQL statement of IBlock
  sprintf(sql, "CREATE TABLE iblock_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      reducer_id     INT       NOT NULL, \
      block_seq      INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id, reducer_id, block_seq));"); 
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("iblock_table created successfully\n");
  }
  sqlite3_close(db);
}

void DirectoryMR::insert_idata_metadata(IDataInsert idata_insert) {
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "INSERT INTO idata_table (\
      job_id, map_id, num_reducer) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      idata_insert.job_id, idata_insert.map_id, idata_insert.num_reducer);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("idata_metadata inserted successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}
void DirectoryMR::insert_igroup_metadata(IGroupInsert igroup_insert) {
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "INSERT INTO igroup_table (\
      job_id, map_id, reducer_id, num_block) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      igroup_insert.job_id, igroup_insert.map_id, igroup_insert.reducer_id,
      igroup_insert.num_block);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("igroup_metadata inserted successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}
void DirectoryMR::insert_iblock_metadata(IBlockInsert iblock_insert) {
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "INSERT INTO iblock_table (\
      job_id, map_id, reducer_id, block_seq) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      iblock_insert.job_id, iblock_insert.map_id, iblock_insert.reducer_id,
      iblock_insert.block_seq);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("iblock_metadata inserted successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}

int DirectoryMR::idata_callback(void *idata_info, int argc, char **argv, char **azColName) 
{
  auto idata = reinterpret_cast<IDataInfo*>(idata_info);
  idata->job_id = atoi(argv[0]);
  idata->map_id = atoi(argv[1]);
  idata->num_reducer = atoi(argv[2]);
  return 0;
}

int DirectoryMR::idata_list_callback(void *list, int argc, char **argv, char **azColName) 
{
    auto idata_list = reinterpret_cast<IDataList*> (list);
    for (int i = 0; i < argc; i++) {
      IDataInfo tmp_idata;
      tmp_idata.job_id = atoi(argv[i++]);
      tmp_idata.map_id = atoi(argv[i++]);
      tmp_idata.num_reducer = atoi(argv[i]); 
      idata_list->data.push_back(tmp_idata);
    }
    return 0;
}

int DirectoryMR::igroup_callback(void *igroup_info, int argc, char **argv,
    char **azColName) {
  auto igroup = reinterpret_cast<IGroupInfo*>(igroup_info);
  igroup->job_id = atoi(argv[0]);
  igroup->map_id = atoi(argv[1]);
  igroup->reducer_id = atoi(argv[2]);
  igroup->num_block = atoi(argv[3]);
  return 0;
}
int DirectoryMR::iblock_callback(void *iblock_info, int argc, char **argv,
    char **azColName) {
  auto iblock = reinterpret_cast<IBlockInfo*>(iblock_info);
  iblock->job_id = atoi(argv[0]);
  iblock->map_id = atoi(argv[1]);
  iblock->reducer_id = atoi(argv[2]);
  iblock->block_seq = atoi(argv[3]);
  return 0;
}
void DirectoryMR::select_idata_metadata(uint32_t job_id, uint32_t map_id,
    IDataInfo *idata_info) {
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "SELECT * from idata_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 ";", job_id, map_id);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, idata_callback, (void*)idata_info, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("idata_metadata selected successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}

void DirectoryMR::select_all_idata_metadata(IDataList &idata_list)
{
     // open database
     open_db();
     mutex.lock();
     // create sql statement
     sprintf(sql, "SELECT * from idata_table;"); 
     // execute sql statement
     rc = sqlite3_exec(db, sql, idata_list_callback, (void*)&idata_list, &zErrMsg);
     if (rc != SQLITE_OK)
     {
       context.logger -> error("SQL error: %s\n", zErrMsg);
       sqlite3_free(zErrMsg);
     } 
     else
     {
       context.logger -> info("idata_metadata selected successfully\n");
     }
     // close database
     sqlite3_close(db);
     mutex.unlock();
}

void DirectoryMR::select_igroup_metadata(uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id, IGroupInfo *igroup_info) {
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "SELECT * from igroup_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 " and reducer_id=%" PRIu32 ";", job_id, map_id,
      reducer_id);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, igroup_callback, (void*)igroup_info, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("igroup_metadata selected successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}

void DirectoryMR::select_iblock_metadata(uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id, uint32_t block_seq, IBlockInfo *iblock_info) {
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "SELECT * from iblock_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 " and reducer_id=%" PRIu32 " and block_seq=%" PRIu32 ";",
      job_id, map_id, reducer_id, block_seq);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, iblock_callback, (void*)iblock_info, &zErrMsg);
  if(rc != SQLITE_OK) {
    context.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    context.logger->info("iblock_metadata selected successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}

}  // namespace eclipse
