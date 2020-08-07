

/**
 * 读取&解析 record
 */

function getNumberFromUint8Array(bin: Uint8Array, ptr: number): number {
    return bin[ptr] + bin[ptr+1]*256 + bin[ptr+2]*256*256 + bin[ptr+3]*256*256*256;
}

class GRecord{
    static readonly Empty = 0;
    static readonly Ready = 1;
    static readonly Playing = 2;
    static readonly Paused = 3;
    static readonly Overed = 4;
    private _ee = {};
    private _it: number = null;
    private _buffer: Uint8Array = null;
    private _status: number = 0;
    private _ptr: number = 0;
    private _speed: number = 1;
    private _id2idx = {};
    private _idx2id = [0,0,0,0];

    private _movePtr () {
        if (this._status != GRecord.Playing) {
            clearTimeout(this._it);
            return;
        }
        if (this._buffer[this._ptr] == 255) {
            clearTimeout(this._it);
            this._status = GRecord.Overed;
            this.emit("gameOver", {uid: 0});
            return;
        }
        let uid = this._idx2id[this._buffer[this._ptr]],
            eid = this._buffer[this._ptr + 1],
            timeout = this._buffer[this._ptr + 2] + this._buffer[this._ptr + 3] * 256;
        this._it = setTimeout(()=>{
            if (eid >= 0 && eid <= 18) {
                this._ptr += 4;
                this.emit("newDropping", {uid: uid, next: eid});
            } else if (eid == 19) {
                this._ptr += 4;
                let pos = getNumberFromUint8Array(this._buffer, this._ptr);
                this._ptr += 4;
                this.emit("posChanged", {uid: uid, pos: pos});
            } else if (eid == 20) {
                this._ptr += 4;
                this.emit("gameOver", {uid: uid});
            } else if (eid >= 21 && eid <= 39) {
                this._ptr += 4;
                this.emit("droppingChanged", {uid: uid, dropping: eid - 21});
            } else if (eid > 63) {
                let rlen = eid - 63;
                this._ptr += 4;
                let reduce: number[] = [];
                for (let i = 0; i < rlen; i++) {
                    reduce.push(getNumberFromUint8Array(this._buffer, this._ptr));
                    this._ptr += 4;
                }
                this.emit("reduce", {reduce: reduce});
            } else {
                console.log("发生错误，在", this._buffer, this._ptr, "附近!");
            }
            this._movePtr();
        }, this._speed === 0 ? 0 : timeout / this._speed);
    }
    
    constructor (record?: Uint8Array) {
        if (record) {
            this.load(record);
        }
    }
    load (record: Uint8Array) {
        this._buffer = new Uint8Array(record);
        this._status = GRecord.Ready;
        this._id2idx = {};
        this._ptr = 0;
        for (let i = 0; i < 4; i++, this._ptr += 4) {
            let uid = getNumberFromUint8Array(record, this._ptr);
            this._id2idx[uid] = i;
            this._idx2id[i] = uid;
        }
    }
    clear () {
        this._buffer = null;
        this._status = GRecord.Empty;
        if (this._it !== null) clearTimeout(this._it);
        this._it = null;
    }
    /**
     * 开始或重新开始
     */
    restart () {
        if (this._status === GRecord.Empty) return;
        this._status = GRecord.Playing;
        this._ptr = 16;
        // emit gameStart unn
        var unn: number[] = [];
        this._idx2id.forEach(uid => {
            unn.push(uid);
            let byte = this._buffer[this._ptr++];
            unn.push(Math.floor(byte / 16));
            unn.push(byte % 16);
        });
        this.emit("gameStart", {unn: unn});
        this._movePtr();
    }

    pause () {
        if (this._it !== null) clearTimeout(this._it);
        this._status = GRecord.Paused;
    }
    play () {
        if (this._status == GRecord.Paused) {
            this._status = GRecord.Playing;
            this._movePtr();
        } else if (this._status == GRecord.Ready) {
            this.restart();
        }
    }
    setSpeed (speed: number) {
        if (speed >= 0)
            this._speed = speed;
    }
    on (en:string, efbk:Function) {
        efbk && (this._ee[en] = this._ee[en] || []).push(efbk);
    }
    emit (en:string, ..._: any[] | null) {
        if(!(en in this._ee)) return;
        var _es = this._ee[en],
            _args = Array.prototype.slice.call(arguments, 1),
            _e;
        for (_e of _es)
            _e.apply(null, _args);
    }
    /**
     * 获取状态
     */
    status () {
        return this._status;
    }
}