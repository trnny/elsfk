/**
 * 在浏览器环境下 无法使用import
 */
// import './msg.js';

/**
 * 检查对象是否符合pb要求 
 * pb: ProtoBuf
 * @param {object} obj 被检查对象
 */
function objcheck (obj) {
    if (typeof obj === "object" && obj.hasOwnProperty("desc") && typeof obj.desc === "string" && obj.desc.length)
    {
        if (obj.hasOwnProperty("data") && typeof obj.data !== "object") {
            return false;
        }
        return true;
    }
    return false;
}

/**
 * 将二进制数组解析为js对象
 * @param {Uint8Array | ArrayBuffer} bin 二进制数组
 */
function bin2jso(bin) {
    return pb2jso(proto.Msg.deserializeBinary(bin));
}

/**
 * 将js对象解析为二进制数组
 * 用于ws发送
 * ws: WebSocket
 * @param {object} jso 
 */
function jso2bin(jso) {
    return jso2pb(jso).serializeBinary();
}

/**
 * 将pb对象解析为js对象
 * @param {proto.Msg} pb pb对象
 */
function pb2jso(pb) {
    if (!(pb instanceof proto.Msg)) throw Error;
    const jso = new Object();
    const pm = pb.getDataMap();
    jso.desc = pb.getDesc();
    jso.data = new Object();
    pm.forEach((v,k)=>{
        jso.data[k] = getpbo(v);
    });
    return jso;
}

/**
 * 将js对象解析为pb对象
 * @param {object} jso 被解析对象
 */
function jso2pb(jso) {
    if (!objcheck(jso)) throw Error;
    const pb = new proto.Msg();
    const pm = pb.getDataMap();
    pb.setDesc(jso.desc);
    if (jso.data) for (var key in jso.data) {
        pm.set(key, makepbv(jso.data[key]));
    }
    return pb;
}

/**
 * 根据js变量生成pb中的值
 * @param {Uint8Array|ArrayBuffer|number[]|string[]|boolean|number|string} o 输入js变量
 */
function makepbv(o) {
    if (o === undefined || o === null) throw TypeError;
    const v = new proto.Msg.VType();
    if (o instanceof Uint8Array) {
        v.setWhich(6);
        v.setVBytes(o);
    }else if(o instanceof ArrayBuffer) {
        v.setWhich(6);
        v.setVBytes(new Uint8Array(o));
    }
    else if (o instanceof Array && o.length) {
        if (typeof o[0] === 'number') {
            v.setWhich(5);
            v.setVrIntList(o);
        }else{
            v.setWhich(4);
            v.setVrStrList(o);
        }
    }else if (typeof o === "boolean") {
        v.setWhich(3);
        v.setVBool(o);
    }else if (typeof o === "number") {
        v.setWhich(2);
        v.setVInt(o);
    }else if (typeof o === "string") {
        v.setWhich(1);
        v.setVStr(o);
    }else {
        v.setWhich(0);  // 空数组
    }
    return v;
}

/**
 * 获取pb中的值
 * @param {proto.Msg.VType} v pb中的值
 */
function getpbo(v) {
    if (v instanceof proto.Msg.VType) {
        switch (v.getWhich()){
        case 0:
            return [];
        case 1:
            return v.getVStr();
        case 2:
            return v.getVInt();
        case 3:
            return v.getVBool();
        case 4:
            return v.getVrStrList();
        case 5:
            return v.getVrIntList();
        case 6:
            return v.getVBytes();
        default:
            throw Error;
        }
    }
    else throw Error;
}

class WS {
    static __wsurl = 'ws://10.10.3.78:8000';
    __ws = new WebSocket(WS.__wsurl);
    __ee = {};
    __ok = false;
    constructor () {
        this.__ws.binaryType = "arraybuffer";
        /**
         * 当ws状态改变时触发事件 无参数 
         * ws启动成功时向服务器发送`wslink` 该消息暂无其他参数，后期可以加上一些其他信息
         */
        this.__ws.onopen = () => {
            this.__ok = true;
            this.emit('wsopen');
            // this.send({desc: 'wslink'});
        }
        this.__ws.onclose = () => {
            this.__ok = false;
            this.emit('wsclose');
        }
        this.__ws.onerror = () => {
            this.emit('wserror');
        }
        this.__ws.onmessage = ev => {
            this.emit('wsmsg', ev.data);
            var jso = bin2jso(ev.data);
            this.emit('wsjso', jso);
            /**
             * 检查消息是否合格  合格则触发消息事件  事件名为消息`desc`字段 参数为消息附带的`data`字段对象
             * 否则触发`wserrormsg`事件 参数为未解析消息
             */
            if (objcheck(jso)) {
                this.emit(jso.desc, jso.data);
            }else{
                this.emit('wserrormsg', ev.data);
            }
        }
    }
    /**
     * 事件注册器
     * @param {string} en 事件名
     * @param {Function} efbk 事件处理器
     * 请为ws注册`wsopen`,`wsclose`,`wserror`,`wserrormsg`,`wsrelink`等事件
     */
    on (en, efbk) {
        efbk && (this.__ee[en] = this.__ee[en] || []).push(efbk);
    }
    /**
     * 事件触发器
     * @param {string} en 事件名
     * @param {arguments} _ 事件参数
     */
    emit (en, ..._) {
        if(!(en in this.__ee)) return;
        var _es = this.__ee[en],
            _args = Array.prototype.slice.call(arguments, 1),
            _e;
        for (_e of _es)
            _e.apply(null, _args);
    }
    /**
     * 发送消息到服务器
     * 若为正确开启则不能发送成功
     * @param {object} obj 消息对象
     * 以账号注册为例 发送消息 ws.send({desc: 'signup', data: {uname: 'todd', age: 21, password: ‘密码md5’, ...其他信息}});
     * 并注册事件 ws.on('signup', data => {
     *      data.ok     // 注册成功否?
     *      data.uid    // 用户账号，用于登陆等
     *      data.idx    // 第多少位注册
     *      // ......
     * });
     * 注意消息的结构 一定是{desc: 'desc字段值', data:{...附带信息}} 其中`desc`字段必需，用以告诉服务器该消息的目的，
     * `data`字段非必须，若无附带信息可以省略 `data`字段对象的结构为键值对的形式 值的类型参照`makepbv`函数
     */
    send (obj) {
        this.__ok && this.__ws.send(jso2bin(obj).buffer);
    }
    /**
     * 重新连接
     * 触发relink事件 无参数
     * 比如网络不好或者其他的可能需要重连
     */
    relink () {
        this.__ws = new WebSocket(WS.__wsurl);
        this.__ws.binaryType = "arraybuffer";
        this.__ok = false;
        this.emit('wsrelink');
    }
}