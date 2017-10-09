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
#include "../common/context_singleton.hh"

#include <cinttypes>
#include <sqlite3.h>
#include <string>

#define DEFAULT_QUERY_SIZE 512

using namespace std;
namespace eclipse {

static int idata_callback(void *idata_info, int argc, 
    char **argv, char **azColName) 
{
  auto idata = reinterpret_cast<IDataInfo*>(idata_info);
  idata->job_id = atoi(argv[0]);
  idata->map_id = atoi(argv[1]);
  idata->num_reducer = atoi(argv[2]);
  return 0;
}

static int idata_list_callback(void *list, int argc, 
    char **argv, char **azColName) 
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

static int igroup_callback(void *igroup_info, int argc, char **argv,
    char **azColName) {
  auto igroup = reinterpret_cast<IGroupInfo*>(igroup_info);
  igroup->job_id = atoi(argv[0]);
  igroup->map_id = atoi(argv[1]);
  igroup->reducer_id = atoi(argv[2]);
  igroup->num_block = atoi(argv[3]);
  return 0;
}
static int iblock_callback(void *iblock_info, int argc, char **argv, 
    char **azColName) {
  auto iblock = reinterpret_cast<IBlockInfo*>(iblock_info);
  iblock->job_id = atoi(argv[0]);
  iblock->map_id = atoi(argv[1]);
  iblock->reducer_id = atoi(argv[2]);
  iblock->block_seq = atoi(argv[3]);
  return 0;
}

void DirectoryMR::create_tables() {
  char sql[DEFAULT_QUERY_SIZE];
  // Create SQL statement of IData
  sprintf(sql, "CREATE TABLE IF NOT EXISTS idata_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      num_reducer    INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id));"); 

  // Execute SQL statement
  if(Directory::query_exec_simple(sql, NULL, NULL))
    DEBUG("idata_table created successfully");

  // Create SQL statement of IGroup
  sprintf(sql, "CREATE TABLE IF NOT EXISTS igroup_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      reducer_id     INT       NOT NULL, \
      num_block      INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id, reducer_id));"); 

  // Execute SQL statement
  if(Directory::query_exec_simple(sql, NULL, NULL))
    DEBUG("igroup_table created successfully");

  // Create SQL statement of IBlock
  sprintf(sql, "CREATE TABLE IF NOT EXISTS iblock_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      reducer_id     INT       NOT NULL, \
      block_seq      INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id, reducer_id, block_seq));"); 

  if(Directory::query_exec_simple(sql, NULL, NULL))
    DEBUG("iblock_table created successfully");
}

void DirectoryMR::insert_idata_metadata(IDataInsert idata_insert) {
  char sql[DEFAULT_QUERY_SIZE];
  // Create sql statement
  sprintf(sql, "INSERT INTO idata_table (\
      job_id, map_id, num_reducer) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      idata_insert.job_id, idata_insert.map_id, idata_insert.num_reducer);

  Directory::query_exec_simple(sql, NULL, NULL);
}
void DirectoryMR::insert_igroup_metadata(IGroupInsert igroup_insert) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "INSERT INTO igroup_table (\
      job_id, map_id, reducer_id, num_block) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      igroup_insert.job_id, igroup_insert.map_id, igroup_insert.reducer_id,
      igroup_insert.num_block);

  Directory::query_exec_simple(sql, NULL, NULL);
}
void DirectoryMR::insert_iblock_metadata(IBlockInsert iblock_insert) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "INSERT INTO iblock_table (\
      job_id, map_id, reducer_id, block_seq) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      iblock_insert.job_id, iblock_insert.map_id, iblock_insert.reducer_id,
      iblock_insert.block_seq);

  Directory::query_exec_simple(sql, NULL, NULL);
}
void DirectoryMR::select_idata_metadata(uint32_t job_id, uint32_t map_id,
    IDataInfo *idata_info) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from idata_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 ";", job_id, map_id);

  if (Directory::query_exec_simple(sql,idata_callback, (void*)idata_info))
    DEBUG("idata_metadata selected successfully\n");
}

void DirectoryMR::select_all_idata_metadata(IDataList &idata_list) {
  char sql[DEFAULT_QUERY_SIZE];

     sprintf(sql, "SELECT * from idata_table;"); 

     if (Directory::query_exec_simple(sql,idata_list_callback, (void*)&idata_list))
       DEBUG("idata_metadata selected successfully");
}

void DirectoryMR::select_igroup_metadata(uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id, IGroupInfo *igroup_info) {
  char sql[DEFAULT_QUERY_SIZE];

  snprintf(sql, DEFAULT_QUERY_SIZE, "SELECT * from igroup_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 " and reducer_id=%" PRIu32 ";", job_id, map_id,
      reducer_id);

  if (Directory::query_exec_simple(sql, igroup_callback, (void*)igroup_info))
    DEBUG("igroup_metadata selected successfully");
}

void DirectoryMR::select_iblock_metadata(uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id, uint32_t block_seq, IBlockInfo *iblock_info) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * from iblock_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 " and reducer_id=%" PRIu32 " and block_seq=%" PRIu32 ";",
      job_id, map_id, reducer_id, block_seq);

  if (Directory::query_exec_simple(sql, iblock_callback, (void*)iblock_info))
    DEBUG("iblock_metadata selected successfully");
}

uint32_t DirectoryMR::select_number_of_reducers(uint32_t job_id) {
  char sql[DEFAULT_QUERY_SIZE];

  sprintf(sql, "SELECT * FROM iblock_table WHERE (job_id='%" PRIu32 "') \
      ORDER BY reducer_id DESC LIMIT 1;", job_id);

  uint32_t ret = 0;
  Directory::query_exec_simple(sql, [] (void* output, int, char **argv, char**) {
    *reinterpret_cast<uint32_t*>(output) = atoi(argv[2]);
    return 0;
  }, (void*)&ret);
  return ret + 1;
}


}  // namespace eclipse
