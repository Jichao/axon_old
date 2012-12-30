//模块可配置参数定义

using namespace axon;

/* 
{
 block_name,
 keyname,
 value_type,
 default_value,
},
*/

static struct conf_item_define_t g_module_conf[] = 
{

{
"root",
"max_online", 
CONF_T_INT,
100,
},

{
"root",
"work_dir",
CONF_T_STR,
"./",
}, 

{
"root",
"logpath",
CONF_T_STR,
"log/",
},



};

