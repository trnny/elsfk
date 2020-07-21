/*eslint-disable block-scoped-var, id-length, no-control-regex, no-magic-numbers, no-prototype-builtins, no-redeclare, no-shadow, no-var, sort-vars*/
(function(global, factory) { /* global define, require, module */

    /* AMD */ if (typeof define === 'function' && define.amd)
        define(["./protobuf"], factory);

    /* CommonJS */ else if (typeof require === 'function' && typeof module === 'object' && module && module.exports)
        module.exports = factory(require("./protobuf"));

    /* Global */ else factory(global.protobuf);

})(this, function($protobuf) {
    "use strict";

    var $Reader = $protobuf.Reader, $Writer = $protobuf.Writer, $util = $protobuf.util;
    
    var $root = $protobuf.roots["default"] || ($protobuf.roots["default"] = {});
    
    $root.Msg = (function() {
    
        function Msg(properties) {
            this.data = {};
            if (properties)
                for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                    if (properties[keys[i]] != null)
                        this[keys[i]] = properties[keys[i]];
        }
    
        Msg.prototype.desc = "";
        Msg.prototype.data = $util.emptyObject;
    
        Msg.encode = function encode(message, writer) {
            if (!writer)
                writer = $Writer.create();
            if (message.desc != null && message.hasOwnProperty("desc"))
                writer.uint32(10).string(message.desc);
            if (message.data != null && message.hasOwnProperty("data"))
                for (var keys = Object.keys(message.data), i = 0; i < keys.length; ++i) {
                    writer.uint32(18).fork().uint32(10).string(keys[i]);
                    $root.Msg.VType.encode(message.data[keys[i]], writer.uint32(18).fork()).ldelim().ldelim();
                }
            return writer;
        };
    
        Msg.decode = function decode(reader, length) {
            if (!(reader instanceof $Reader))
                reader = $Reader.create(reader);
            var end = length === undefined ? reader.len : reader.pos + length, message = new $root.Msg(), key;
            while (reader.pos < end) {
                var tag = reader.uint32();
                switch (tag >>> 3) {
                case 1:
                    message.desc = reader.string();
                    break;
                case 2:
                    reader.skip().pos++;
                    if (message.data === $util.emptyObject)
                        message.data = {};
                    key = reader.string();
                    reader.pos++;
                    message.data[key] = $root.Msg.VType.decode(reader, reader.uint32());
                    break;
                default:
                    reader.skipType(tag & 7);
                    break;
                }
            }
            return message;
        };
    
        Msg.VType = (function() {
    
            function VType(properties) {
                this.vrStr = [];
                this.vrInt = [];
                if (properties)
                    for (var keys = Object.keys(properties), i = 0; i < keys.length; ++i)
                        if (properties[keys[i]] != null)
                            this[keys[i]] = properties[keys[i]];
            }
    
            VType.prototype.vStr = "";
            VType.prototype.vInt = 0;
            VType.prototype.vBool = false;
            VType.prototype.vrStr = $util.emptyArray;
            VType.prototype.vrInt = $util.emptyArray;
            VType.prototype.vBytes = $util.newBuffer([]);
            VType.prototype.which = 0;
    
            VType.encode = function encode(message, writer) {
                if (!writer)
                    writer = $Writer.create();
                if (message.vStr != null && message.hasOwnProperty("vStr"))
                    writer.uint32(10).string(message.vStr);
                if (message.vInt != null && message.hasOwnProperty("vInt"))
                    writer.uint32(16).int32(message.vInt);
                if (message.vBool != null && message.hasOwnProperty("vBool"))
                    writer.uint32(24).bool(message.vBool);
                if (message.vrStr != null && message.vrStr.length)
                    for (var i = 0; i < message.vrStr.length; ++i)
                        writer.uint32(34).string(message.vrStr[i]);
                if (message.vrInt != null && message.vrInt.length) {
                    writer.uint32(42).fork();
                    for (var i = 0; i < message.vrInt.length; ++i)
                        writer.int32(message.vrInt[i]);
                    writer.ldelim();
                }
                if (message.vBytes != null && message.hasOwnProperty("vBytes"))
                    writer.uint32(50).bytes(message.vBytes);
                if (message.which != null && message.hasOwnProperty("which"))
                    writer.uint32(56).int32(message.which);
                return writer;
            };
    
            VType.decode = function decode(reader, length) {
                if (!(reader instanceof $Reader))
                    reader = $Reader.create(reader);
                var end = length === undefined ? reader.len : reader.pos + length, message = new $root.Msg.VType();
                while (reader.pos < end) {
                    var tag = reader.uint32();
                    switch (tag >>> 3) {
                    case 1:
                        message.vStr = reader.string();
                        break;
                    case 2:
                        message.vInt = reader.int32();
                        break;
                    case 3:
                        message.vBool = reader.bool();
                        break;
                    case 4:
                        if (!(message.vrStr && message.vrStr.length))
                            message.vrStr = [];
                        message.vrStr.push(reader.string());
                        break;
                    case 5:
                        if (!(message.vrInt && message.vrInt.length))
                            message.vrInt = [];
                        if ((tag & 7) === 2) {
                            var end2 = reader.uint32() + reader.pos;
                            while (reader.pos < end2)
                                message.vrInt.push(reader.int32());
                        } else
                            message.vrInt.push(reader.int32());
                        break;
                    case 6:
                        message.vBytes = reader.bytes();
                        break;
                    case 7:
                        message.which = reader.int32();
                        break;
                    default:
                        reader.skipType(tag & 7);
                        break;
                    }
                }
                return message;
            };
    
            return VType;
        })();
        
        return Msg;
    })();

    return $root;
});