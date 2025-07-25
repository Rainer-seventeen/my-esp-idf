/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NET_H_
#define _NET_H_

#include "mesh/access.h"
#include "mesh/mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_NET_FLAG_KR        BIT(0)
#define BLE_MESH_NET_FLAG_IVU       BIT(1)

#define BLE_MESH_KR_NORMAL          0x00    // 正常状态，使用旧密钥
#define BLE_MESH_KR_PHASE_1         0x01    // 过渡状态，使用 old 发送，允许接受 new 和 old
#define BLE_MESH_KR_PHASE_2         0x02    // 激活状态，使用 new 发送，允许接受 new 和 old
#define BLE_MESH_KR_PHASE_3         0x03    // 结束状态，使用 new 发送，只允许接受 new

#define BLE_MESH_IV_UPDATE(flags)   ((flags >> 1) & 0x01)
#define BLE_MESH_KEY_REFRESH(flags) (flags & 0x01)

/* How many hours in between updating IVU duration */
#define BLE_MESH_IVU_MIN_HOURS      96
#define BLE_MESH_IVU_HOURS          (BLE_MESH_IVU_MIN_HOURS / CONFIG_BLE_MESH_IVU_DIVIDER)
#define BLE_MESH_IVU_TIMEOUT        K_HOURS(BLE_MESH_IVU_HOURS)

struct bt_mesh_app_key {
    uint16_t net_idx;
    uint16_t app_idx;
    bool     updated;
    struct bt_mesh_app_keys {
        uint8_t id;
        uint8_t val[16];
    } keys[2];
};

struct bt_mesh_subnet {
    uint32_t snb_sent;                  /*  Timestamp of last sent secure network beacon */
    uint8_t  snb_last;                  /*  Number of secure network beacons during last observation window */
    uint8_t  snb_cur;                   /*  Number of secure network beacons observed during currently ongoing window.*/
    uint8_t  snb_cache[21];             /*  Cached last receive authenticated secure beacon */

#if CONFIG_BLE_MESH_PRIVATE_BEACON
    uint32_t mpb_sent;                  /* Timestamp of last sent private beacon */
    uint8_t  mpb_last;                  /* Number of private beacons during last observation window */
    uint8_t  mpb_cur;                   /* Number of private beacons observed during currently ongoing window. */
    uint8_t  mpb_cache[21];             /* Cached last receive private beacon (Random + Authentication Tag) */

    uint8_t  mpb_flags_last;            /* Flags of last sent private beacon */
    uint8_t  mpb_ivi_last: 1;           /* IV Index of last sent private beacon */
    uint8_t  mpb_random[13];            /* Random of current private beacon */

    uint8_t  private_node_id;           /* Private Node Identity State */
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */

    uint16_t net_idx;                   /* NetKeyIndex */

#if CONFIG_BLE_MESH_BRC_SRV
    uint16_t sbr_net_idx;               /* NetKeyIndex of bridged subnet */
#endif

    bool     kr_flag;                   /* Key Refresh Flag */
    uint8_t  kr_phase;                  /* Key Refresh Phase */

    uint8_t  node_id;                   /* Node Identity State */
    uint32_t node_id_start;             /* Node Identity started timestamp */

    uint8_t  auth[8];                   /* Beacon Authentication Value */

    struct bt_mesh_subnet_keys {
        uint8_t net[16];                /* NetKey */
        uint8_t nid;                    /* NID */
        uint8_t enc[16];                /* EncKey */
        uint8_t net_id[8];              /* Network ID */
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
        uint8_t identity[16];           /* IdentityKey */
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
        uint8_t privacy[16];            /* PrivacyKey */
        uint8_t beacon[16];             /* BeaconKey */
#if CONFIG_BLE_MESH_PRIVATE_BEACON
        uint8_t private_beacon[16];     /* Private BeaconKey */
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
#if CONFIG_BLE_MESH_DF_SRV
        uint8_t direct_nid;             /* Directed NID */
        uint8_t direct_enc[16];         /* Directed EncKey */
        uint8_t direct_privacy[16];     /* Directed PrivacyKey */
#endif /* CONFIG_BLE_MESH_DF_SRV */
    } keys[2];

    /* Indicate if proxy privacy is enabled (i.e. sending Mesh Private Beacons
     * or Secure Network Beacons) to proxy client.
     *
     * Note: in Mesh Spec, it describes as "the Proxy Privacy parameter for the
     * connection". Here we put the parameter in the subnet, since when sending
     * mesh beacon, and for the existing subnets, proxy server should send mesh
     * beacon for each of the subnets.
     */
    uint8_t proxy_privacy;

#if CONFIG_BLE_MESH_DF_SRV
    uint8_t directed_forwarding;
    uint8_t directed_relay; /* Binding with Directed Forwarding state */
    uint8_t directed_proxy; /* Binding with Directed Forwarding state & GATT Proxy state */
    uint8_t directed_proxy_use_default; /* Binding with Directed Proxy state */
    uint8_t directed_friend;

    uint8_t use_directed;
    struct {
        uint16_t len_present:1,
                 range_start:15;
        uint8_t  range_length;
    } proxy_client_uar;

    uint8_t path_metric_type:3,
            path_lifetime_type:2,
            two_way_path:1;

    uint8_t forward_number;

    /* The Discovery Table initially is empty. A Path Origin updates
     * its Discovery Table when a Directed Forwarding Initialization
     * procedure is executed. A Path Target or a Directed Relay node
     * updates its Discovery Table when a PATH_REQUEST message is
     * received and processed.
     */
    struct bt_mesh_discovery_table {
        uint8_t max_disc_entries;
        uint8_t max_concurr_init;   /* default is 0x02 */
        uint8_t concurr_init;

        bt_mesh_mutex_t mutex;
        sys_slist_t entries;
    } discovery_table;

    struct bt_mesh_forward_table {
        uint8_t max_ford_entries;
        uint8_t max_deps_nodes;
        uint16_t  update_id;

        bt_mesh_mutex_t mutex;
        sys_slist_t entries;
    } forward_table;

    uint8_t wanted_lanes;

    uint8_t unicast_echo_interval;

    uint8_t multicast_echo_interval;
#endif /* CONFIG_BLE_MESH_DF_SRV */
};

struct bt_mesh_rpl {
    uint16_t src;
    bool     old_iv;
#if CONFIG_BLE_MESH_SETTINGS
    bool     store;
#endif
    uint32_t seq;
};

#if CONFIG_BLE_MESH_FRIEND
#define FRIEND_SEG_RX           CONFIG_BLE_MESH_FRIEND_SEG_RX
#define FRIEND_SUB_LIST_SIZE    CONFIG_BLE_MESH_FRIEND_SUB_LIST_SIZE
#else
#define FRIEND_SEG_RX           0
#define FRIEND_SUB_LIST_SIZE    0
#endif

struct bt_mesh_friend {
    uint16_t lpn;
    uint8_t  recv_delay;
    uint8_t  fsn:1,
             send_last:1,
             pending_req:1,
             pending_buf:1,
             valid:1,
             established:1;
    int32_t  poll_to;
    uint8_t  num_elem;
    uint16_t lpn_counter;
    uint16_t counter;

    uint16_t net_idx;

    uint16_t sub_list[FRIEND_SUB_LIST_SIZE];

    struct k_delayed_work timer;

    struct bt_mesh_friend_seg {
        sys_slist_t queue;

        /* The target number of segments, i.e. not necessarily
         * the current number of segments, in the queue. This is
         * used for Friend Queue free space calculations.
         */
        uint8_t seg_count;
    } seg[FRIEND_SEG_RX];

    struct net_buf *last;

    sys_slist_t queue;
    uint32_t    queue_size;

    /* Friend Clear Procedure */
    struct {
        uint32_t start;               /* Clear Procedure start */
        uint16_t frnd;                /* Previous Friend's address */
        uint16_t repeat_sec;          /* Repeat timeout in seconds */
        struct k_delayed_work timer;  /* Repeat timer */
    } clear;
};

#if CONFIG_BLE_MESH_LOW_POWER
#define LPN_GROUPS CONFIG_BLE_MESH_LPN_GROUPS
#else
#define LPN_GROUPS 0
#endif

/* Low Power Node state */
struct bt_mesh_lpn {
    enum __attribute__((packed)) {
        BLE_MESH_LPN_DISABLED,     /* LPN feature is disabled */
        BLE_MESH_LPN_CLEAR,        /* Clear in progress */
        BLE_MESH_LPN_TIMER,        /* Waiting for auto timer expiry */
        BLE_MESH_LPN_ENABLED,      /* LPN enabled, but no Friend */
        BLE_MESH_LPN_REQ_WAIT,     /* Wait before scanning for offers */
        BLE_MESH_LPN_WAIT_OFFER,   /* Friend Req sent */
        BLE_MESH_LPN_ESTABLISHED,  /* Friendship established */
        BLE_MESH_LPN_RECV_DELAY,   /* Poll sent, waiting ReceiveDelay */
        BLE_MESH_LPN_WAIT_UPDATE,  /* Waiting for Update or message */
        BLE_MESH_LPN_OFFER_RECV,   /* Friend offer received */
    } state;

    /* Transaction Number (used for subscription list) */
    uint8_t xact_next;
    uint8_t xact_pending;
    uint8_t sent_req;

    /* Address of our Friend when we're a LPN. Unassigned if we don't
     * have a friend yet.
     */
    uint16_t frnd;

    /* Value from the friend offer */
    uint8_t  recv_win;

    uint8_t  req_attempts;     /* Number of Request attempts */

    int32_t  poll_timeout;

    uint8_t  groups_changed: 1, /* Friend Subscription List needs updating */
             pending_poll: 1,   /* Poll to be sent after subscription */
             disable: 1,        /* Disable LPN after clearing */
             fsn: 1,            /* Friend Sequence Number */
             established: 1,    /* Friendship established */
             clear_success: 1;  /* Friend Clear Confirm received */

    /* Friend Queue Size */
    uint8_t  queue_size;

    /* LPNCounter */
    uint16_t counter;

    /* Previous Friend of this LPN */
    uint16_t old_friend;

#if CONFIG_BLE_MESH_DF_SRV
    uint8_t  old_directed_forwarding;
#endif

    /* Duration reported for last advertising packet */
    uint16_t adv_duration;

    /* Next LPN related action timer */
    struct k_delayed_work timer;

    /* Subscribed groups */
    uint16_t groups[LPN_GROUPS];

    /* Bit fields for tracking which groups the Friend knows about */
    BLE_MESH_ATOMIC_DEFINE(added, LPN_GROUPS);
    BLE_MESH_ATOMIC_DEFINE(pending, LPN_GROUPS);
    BLE_MESH_ATOMIC_DEFINE(to_remove, LPN_GROUPS);
};

/* bt_mesh_net.flags */
enum {
    BLE_MESH_NODE,            /* Device is a node */
    BLE_MESH_PROVISIONER,     /* Device is a Provisioner */
    BLE_MESH_VALID,           /* We have been provisioned */
    BLE_MESH_VALID_PROV,      /* Provisioner has been enabled */
    BLE_MESH_SUSPENDED,       /* Network is temporarily suspended */
    BLE_MESH_IVU_IN_PROGRESS, /* IV Update in Progress */
    BLE_MESH_IVU_INITIATOR,   /* IV Update initiated by us */
    BLE_MESH_IVU_TEST,        /* IV Update test mode */
    BLE_MESH_IVU_PENDING,     /* Update blocked by SDU in progress */

    /* pending storage actions, must reside within first 32 flags */
    BLE_MESH_RPL_PENDING,
    BLE_MESH_KEYS_PENDING,
    BLE_MESH_NET_PENDING,
    BLE_MESH_IV_PENDING,
    BLE_MESH_SEQ_PENDING,
    BLE_MESH_HB_PUB_PENDING,
    BLE_MESH_CFG_PENDING,
    BLE_MESH_MOD_PENDING,
    BLE_MESH_VA_PENDING,

    /* Don't touch - intentionally last */
    BLE_MESH_FLAG_COUNT,
};

struct bt_mesh_net {
    uint32_t iv_index; /* Current IV Index */
    uint32_t seq;      /* Next outgoing sequence number (24 bits) */

    BLE_MESH_ATOMIC_DEFINE(flags, BLE_MESH_FLAG_COUNT);

    /* Local network interface */
    sys_slist_t local_queue;

#if CONFIG_BLE_MESH_FRIEND
    /* Friend state, unique for each LPN that we're Friends for */
    struct bt_mesh_friend frnd[CONFIG_BLE_MESH_FRIEND_LPN_COUNT];
#endif

#if CONFIG_BLE_MESH_LOW_POWER
    struct bt_mesh_lpn lpn;  /* Low Power Node state */
#endif

    /* Number of hours in current IV Update state */
    uint8_t ivu_duration;

    /* Timer to track duration in current IV Update state */
    struct k_delayed_work ivu_timer;

    uint8_t dev_key[16];    /* Device Key */
    uint8_t dev_key_ca[16]; /* Device Key Candidate */

    struct bt_mesh_app_key app_keys[CONFIG_BLE_MESH_APP_KEY_COUNT];

    struct bt_mesh_subnet sub[CONFIG_BLE_MESH_SUBNET_COUNT];

    struct bt_mesh_rpl rpl[CONFIG_BLE_MESH_CRPL];

#if CONFIG_BLE_MESH_PROVISIONER
    /* Application keys stored by provisioner */
    struct bt_mesh_app_key *p_app_keys[CONFIG_BLE_MESH_PROVISIONER_APP_KEY_COUNT];
    /* Next app_idx can be assigned */
    uint16_t p_app_idx_next;

    /* Network keys stored by provisioner */
    struct bt_mesh_subnet *p_sub[CONFIG_BLE_MESH_PROVISIONER_SUBNET_COUNT];
    /* Next net_idx can be assigned */
    uint16_t p_net_idx_next;
#endif
};

/* Network interface */
enum bt_mesh_net_if {
    BLE_MESH_NET_IF_ADV,
    BLE_MESH_NET_IF_LOCAL,
    BLE_MESH_NET_IF_PROXY,
    BLE_MESH_NET_IF_PROXY_CFG,
};

#define BLE_MESH_NONE_BEARER    0
#define BLE_MESH_ADV_BEARER     BIT(0)
#define BLE_MESH_GATT_BEARER    BIT(1)
#define BLE_MESH_LOCAL_BEARER   BIT(2)
#define BLE_MESH_ALL_BEARERS    (BLE_MESH_ADV_BEARER | BLE_MESH_GATT_BEARER)

/* Decoding context for Network/Transport data */
struct bt_mesh_net_rx {
    struct bt_mesh_subnet *sub;
    struct bt_mesh_msg_ctx ctx;
    uint32_t seq;            /* Sequence Number */
    uint16_t old_iv:1,       /* iv_index - 1 was used */
             new_key:1,      /* Data was encrypted with updated key */
             friend_cred:1 __attribute__((deprecated)), /* Data was encrypted with friend cred */
             ctl:1,          /* Network Control */
             net_if:2,       /* Network interface */
             local_match:1,  /* Matched a local element */
             friend_match:1, /* Matched an LPN we're friends for */
#if CONFIG_BLE_MESH_NOT_RELAY_REPLAY_MSG
             replay_msg:1,   /* Replayed messages */
#endif
             sbr_rpl:1;      /* Bridge RPL attacker */
    uint16_t msg_cache_idx;  /* Index of entry in message cache */
};

/* Encoding context for Network/Transport data */
struct bt_mesh_net_tx {
    struct bt_mesh_subnet *sub;
    struct bt_mesh_msg_ctx *ctx;
    uint16_t src;
    uint8_t  xmit;
    uint8_t  friend_cred:1 __attribute__((deprecated)),
             aszmic:1,
             aid:6;
};

extern struct bt_mesh_net bt_mesh;

#define BLE_MESH_NET_IVI_TX         (bt_mesh.iv_index - \
                                     bt_mesh_atomic_test_bit(bt_mesh.flags, \
                                                             BLE_MESH_IVU_IN_PROGRESS))
#define BLE_MESH_NET_IVI_RX(rx)     (bt_mesh.iv_index - (rx)->old_iv)

#define BLE_MESH_NET_HDR_LEN        9

// 宏定义函数拆解 PDU 的某几个位
#define BLE_MESH_NET_HDR_IVI(pdu)   ((pdu)[0] >> 7)
#define BLE_MESH_NET_HDR_NID(pdu)   ((pdu)[0] & 0x7F)
#define BLE_MESH_NET_HDR_CTL(pdu)   ((pdu)[1] >> 7)
#define BLE_MESH_NET_HDR_TTL(pdu)   ((pdu)[1] & 0x7F)
#define BLE_MESH_NET_HDR_SEQ(pdu)   (sys_get_be24(&(pdu)[2]))
#define BLE_MESH_NET_HDR_SRC(pdu)   (sys_get_be16(&(pdu)[5]))
#define BLE_MESH_NET_HDR_DST(pdu)   (sys_get_be16(&(pdu)[7]))


// unicast address 是 15 位，第一位永远是 0
void bt_mesh_msg_cache_clear(uint16_t unicast_addr, uint8_t elem_num);

// 更改一个子网 key 结构体 bt_mesh_subnet_keys
int bt_mesh_net_keys_create(struct bt_mesh_subnet_keys *keys,
                            const uint8_t key[16]);

/*  根据传入的 NetKey、NetKey 索引（NetIdx）、IV Index 和标志位，创建并初始化一个 BLE Mesh 子网（subnet）
    并完成密钥派生、IV Index 设置、节点身份状态配置等初始化工作。*/
int bt_mesh_net_create(uint16_t idx, uint8_t flags, const uint8_t key[16],
                       uint32_t iv_index);

// 更新子网的flags，并返回当前的 flags
uint8_t bt_mesh_net_flags(struct bt_mesh_subnet *sub);

/*  key refresh 有三个阶段，1配置客户端发送新key，2节点收到后保存新key（此时旧key仍然可用），3用新key发送*/
/*  bt_mesh_kr_update() 通过分析 Beacon 的 KR 标志和是否使用新密钥，
    判断是否需要将本地子网的 Key Refresh 状态推进（进入 Phase 2 或回到 Normal），
    并在必要时调用 bt_mesh_net_revoke_keys() 正式完成密钥切换。*/
bool bt_mesh_kr_update(struct bt_mesh_subnet *sub, uint8_t new_kr, bool new_key);

// 撤销子网的 OldKey，通常在 Key Refresh 过程中使用
void bt_mesh_net_revoke_keys(struct bt_mesh_subnet *sub);

/*  根据子网当前状态选择使用旧或新密钥，生成并更新 Secure Network Beacon 的认证值（Auth），
    以保证 Mesh 网络中的 Beacon 广播具有认证和同步能力。*/
int bt_mesh_net_secure_beacon_update(struct bt_mesh_subnet *sub);


bool bt_mesh_net_iv_update(uint32_t iv_index, bool iv_update);

void bt_mesh_net_sec_update(struct bt_mesh_subnet *sub);

// 根据网络索引获取子网
struct bt_mesh_subnet *bt_mesh_subnet_get(uint16_t net_idx);

// 根据网络 ID、标志位、IV Index 和认证值查找子网
struct bt_mesh_subnet *bt_mesh_subnet_find_with_snb(const uint8_t net_id[8], uint8_t flags,
                                                    uint32_t iv_index, const uint8_t auth[8],
                                                    bool *new_key);

// 将网络层上层的数据打包为 Network PDU 并进行加密与混淆，准备通过 Bearer 层广播或转发。
int bt_mesh_net_encode(struct bt_mesh_net_tx *tx, struct net_buf_simple *buf,
                       bool proxy);

// spec P77
int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
                     const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_net_resend(struct bt_mesh_subnet *sub, struct net_buf *buf,
                       bool new_key, uint8_t *tx_cred, uint8_t tx_tag,
                       const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_net_decode(struct net_buf_simple *data, enum bt_mesh_net_if net_if,
                       struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);

void bt_mesh_generic_net_recv(struct net_buf_simple *data,
                              struct bt_mesh_net_rx *rx,
                              enum bt_mesh_net_if net_if);

static inline void bt_mesh_net_recv(struct net_buf_simple *data, int8_t rssi,
                                    enum bt_mesh_net_if net_if)
{
    struct bt_mesh_net_rx rx = { .ctx.recv_rssi = rssi };
    bt_mesh_generic_net_recv(data, &rx, net_if);
}

bool bt_mesh_primary_subnet_exist(void);

uint32_t bt_mesh_next_seq(void);

void bt_mesh_net_start(void);

void bt_mesh_net_init(void);
void bt_mesh_net_reset(void);
void bt_mesh_net_deinit(void);

void bt_mesh_net_header_parse(struct net_buf_simple *buf,
                              struct bt_mesh_net_rx *rx);

/* Friendship Credential Management */
struct friend_cred {
    uint16_t net_idx;
    uint16_t addr;

    uint16_t lpn_counter;
    uint16_t frnd_counter;

    struct {
        uint8_t nid;         /* NID */
        uint8_t enc[16];     /* EncKey */
        uint8_t privacy[16]; /* PrivacyKey */
    } cred[2];
};

int friend_cred_get(struct bt_mesh_subnet *sub, uint16_t addr, uint8_t *nid,
                    const uint8_t **enc, const uint8_t **priv);
int friend_cred_set(struct friend_cred *cred, uint8_t idx, const uint8_t net_key[16]);
void friend_cred_refresh(uint16_t net_idx);
int friend_cred_update(struct bt_mesh_subnet *sub);
struct friend_cred *friend_cred_create(struct bt_mesh_subnet *sub, uint16_t addr,
                                       uint16_t lpn_counter, uint16_t frnd_counter);
void friend_cred_clear(struct friend_cred *cred);
int friend_cred_del(uint16_t net_idx, uint16_t addr);

static inline void send_cb_finalize(const struct bt_mesh_send_cb *cb,
                                    void *cb_data)
{
    if (!cb) {
        return;
    }

    if (cb->start) {
        cb->start(0, 0, cb_data);
    }

    if (cb->end) {
        cb->end(0, cb_data);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _NET_H_ */
