// ========= 说明 =========

// 1、消息方向
//     前端向后端：   前端主动发起请求
//     后端向前端：   前端请求的返回  &  后端转发某些消息到前端  &  后端主动发送指令
// 2、结构上，消息都是 用于描述消息的字符串`desc`字段 + 形如`Key: Value`的附加消息`data`字段  data字段可以空也可以省略，但desc字段必需，且不为空
// 3、前端发起的所有请求，
//     服务器接收到后不论请求是否成功都会返回消息告知 返回的`desc`字段与请求的保持一致
//     前端都应该为它注册事件 事件名为`desc`字段值  事件参数`data`至少会有`data.ok`属性用以表示请求是否成功  不同事件的事件参数其他属性不同
//     故 前端应该在注册事件的处理函数中进行 消息发送的后续处理 而不是执行了 ws.send(...)之后。
// 4、游戏中，例如下落块旋转，发送消息带上房间号和自己在房间中的编号 服务器转发到房间的所有玩家(包括发起者) 所以在接收到旋转消息时需判断玩家编号

// ========= 结构 =========

// desc值: data值 的形式约定双端消息的结构 实现时请进行参照
// 以下所有都是部分信息，且只是模板   后面需要双方完善&改进  但双端保持同步

// ============ 发送 ============
// 有些消息里 不需要带上自己的`uid`  例如`friendlist` 服务器是根据消息的来源,而不是消息里附带的`uid`来拉取用户的好友列表
// 消息无法发送二维数组 若有二维数组发送的需要 两端统一编码方式即可
wslink: {   // 告知服务器一些自己的信息
}
signup: {   // 注册 附带注册信息
    uname,
    password
}
signin: {   // 登陆 
    uid,
    password
}
userrename: {       // 改名
    newuname
}
roomjoin: {         // 请求加入房间
    hardlevel
}
roomjoinsuccess: {  // 成功加入房间
    plays,
    hardlevel
}
gamectrl: {     // 游戏控制  自己发出 服务器帮忙转发到房间所有玩家
    roomid,
    ctrl        // 标识哪种控制 比如说pause left reduce等等
}
gamectrlrec: {  // 收到游戏控制
    uid,        // 标识哪个玩家 可以是自己
    roomid,
    ctrl
}
friendadd: {    // 加好友
    fuid        // 表示哪个好友
}
friendaddrec: { // 接受好友请求
    fuid
}
friendaddacc: { // 接受好友请求
    fuid
}
frienddel: {    // 删好友
    fuid
}
friendlist: {   // 好友列表
}
friendinfo: {   // 好友信息
    fuid
}
recordlist: {   // 对局记录列表
}
recordinfo: {   // 某条对局记录的对局信息
    recordid
}
recordget: {    // 拉取某条对局记录
    recordid
}


// =========== 接收 ===========
