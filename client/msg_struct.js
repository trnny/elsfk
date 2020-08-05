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

signup: {   // 注册 附带注册信息
    uname,
    password
}
signin: {   // 登陆 
    uid,//int
    password
}

matching: {         // 请求加入房间
}
matchingSure:{      // 
}
newDropping: {
}

reduce: {
    reduce  // 
}

posChanged: {
    pos  // int: y*256+x
}

droppingChanged: {
    dropping  // int
}

gameOver: {
}

recordList: {
}
recordGet: {
    recordId // string
}

score: {
    score   // int
}

rank: {

}


// =========== 接收 ===========


signup: {
    ok,
    uid,
    msg
}

signin: {
    ok
    msg
    rid
}

matching: {
    ok
}
matchingSure : {        // 确认
    ok
    uid
}
matchingSucceed: {
    uids // [uid, uid ...] 长度4 玩家们
}
matchingCancel: {
}

gameStart: {
    unn // [uid,now,next ... ] 长度12 玩家们和他们的块
}

gameOver: {
    ok
    uid // 0表示全结束
}


newDropping: {
    ok
    uid
    next
}

reduce: {
    ok
    uid
    reduce  // 先降成一维int数组再发送
}

posChanged: {
    ok
    uid
    pos  // int: y*256+x
}

droppingChanged: {
    ok
    uid
    dropping  // int:
}

recordList: {
    ok
    ids // string[] 记录标号的数组 长度为记录条数
    dates // string[] 日期数组
    uid1s // string[]
    uid2s
    uid3s
    uid4s
}
recordGet: {
    ok
    record // bin  长度为0表示没有该记录
}

crowd: {
}

rank: {
    uids    // int[]
    scores  // int[]
}


// record

gameOver : {
    uid
}

gameStart: {
    unn
}

newDropping: {
    uid
    next
}

reduce: {
    uid 
    reduce
}

posChanged: {
    uid
    pos
}

droppingChanged: {
    uid
    dropping
}