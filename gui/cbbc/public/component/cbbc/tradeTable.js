class TradeTable extends React.Component {
  static propTypes = {
    data2: PropTypes.array,
    data3: PropTypes.object,
    data4: PropTypes.array,
    data5: PropTypes.array,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.handleClick = this.handleClick.bind(this)
    this.handleClick2 = this.handleClick2.bind(this)
    this.handleChange = this.handleChange.bind(this)
    this.state = {}
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidUpdate() {
    var tb = $('.table-trade')
    var w = 0
    $('.table-trade > colgroup > col').each(function(k, v) {
      if(!($(this).hasClass('d-none')))
        w += parseInt($(this).attr('width'))
    })
    $('.table-trade').css({width: w+'px'})
  }
  
  handleClick() {
    var no = event.target.attributes.getNamedItem('data-no').value,
        name = event.target.name,
        states = this.props.getStates(),
        obj = $.extend(true, {}, states.cells[no].action),
        obj2 = $.extend(true, [], states.cellsConfig),
        type = obj2[no].type
        
    // 校验数据
    var isError = false
    for (const [k, v] of Object.entries(obj)) {
      if(v.hasOwnProperty("valid")) {
        v.feedback = validate(this.props.lang, v.valid, v.value, true)
        if (v.feedback.length>0)
          isError = true
      }
    }
    
    // 顯示 & 隐藏
    if (name == 'show') {
      if (obj2[no].isVisable == true)
        obj2[no].isVisable = false
      else if (obj2[no].isVisable == false)
        obj2[no].isVisable = true
      this.props.setStates({cellsConfig: obj2})
    }
    
    else if (!isError && name == 'isSet') {
      var format1 = (val) => {return parseFloat(formatInputUnit(val, true))}
      var command1 = {
        cmd: "set",
        wtype: type,
        symbol: obj.symbol.value,
        warrant_code: parseInt(obj.code.value),
        issuersize: format1(obj.issuerSize.value),
        quantity: formatLongV2(format1(obj.quantity.value)),
        spread: formatLongV2(obj.spread.value),
        delta: (obj.delta.value).toString(),
        ref: states.prefix+no,
        id: parseInt(states.userId),
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
    }
    
    else if (!isError && name == 'isStart') {
        var command1 = {
          cmd: "start",
          action: "start",
          warrant_code: parseInt(obj.code.value),
          ref: states.prefix+no,
          id: parseInt(states.userId),
          algo_name: states.modules[type]
        }
        sendWebsocket(JSON.stringify(command1))
    }
    
    else if (!isError && name == 'isPause') {
      var command1 = {
        cmd: "pause",
        action:"pause",
        warrant_code: parseInt(obj.code.value),
        ref: states.prefix+no,
        id: parseInt(states.userId),
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
    }
    
    else if (!isError && name == 'isStop') {
      var command1 = {
        cmd: "stop",
        action: "stop",
        warrant_code: parseInt(obj.code.value),
        ref: states.prefix+no,
        id: parseInt(states.userId),
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
    }
  }
  
  handleClick2() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states)
    var code = obj.cells[no].action.code.value
    
    var obj2 = $.extend(true, [], states.cellsConfig)
    var type = obj2[no].type

    if (name == 'isSet2') {
      for (var k of ['wintick', 'stoplost', 'buyoffset', 'selloffset']) {
        var {feedback, responseResult, value} = obj.cells[no].setting[k]
        if (feedback.length == 0) {
          var command1 = {
            cmd: "param",
            action: "param",
            warrant_code: parseInt(code),
            type: k.toUpperCase(),
            value: value,
            ref: obj.prefix+no,
            id: states.userId,
            algo_name: states.modules[type]
          }
          sendWebsocket(JSON.stringify(command1))
        }
      }
    }
    
    else if (name == 'lvlon' || name == 'rtData') {
      if (parseInt(obj.cells[no].setting[name].value) == 1)
        obj.cells[no].setting[name].value = '0'
      else
        obj.cells[no].setting[name].value = '1'
      
      var command1 = {
        cmd: "param",
        action: "param",
        warrant_code: parseInt(code),
        type: name.toUpperCase(),
        value: obj.cells[no].setting[name].value,
        ref: obj.prefix+no,
        id: states.userId,
        algo_name: states.modules[type]
      }
      sendWebsocket(JSON.stringify(command1))
      this.props.setStates({cells: obj.cells})
    }
    
    else if (name == 'remove') {
      obj2[no].isVisable2 = false
      this.props.setStates({cellsConfig: obj2})
    }
  }
  
  handleChange() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var {name, value} = event.target
    var states = this.props.getStates()
    
    if (name == 'quantity' || name == 'delta') {
      var obj = $.extend(true, {}, states.cells[no])
      // 校验数据
      obj.action[name].value = formatInput(value)
      obj.action[name].feedback = validate(this.props.lang, obj.action[name].valid, value, false)
      // 如是输入框, 清除样式
      if ('responseResult' in obj.action[name])
        obj.action[name].responseResult = ''
      // 如stop后更改了data，必须重新set盘
      if (obj.action.status.isStop)
        obj.action.status.needToSetAgain = true
      // 如stop后更改了code，必须清price table
      if (obj.action.status.isStop && name == 'code')
        obj.priceTable = {}
      states.cells[no] = obj
      this.props.setStates({cells: states.cells})
    }
    
    else if (name == 'wintick' || name == 'stoplost' || name == 'buyoffset' || name == 'selloffset') {
      var obj = $.extend(true, {}, states.cells[no].setting)
      obj[name].value = value
      obj[name].feedback = validate(this.props.lang, obj[name].valid, value, true)
      obj[name].responseResult = ''
      states.cells[no].setting = obj
      
      this.props.setStates({cells: states.cells})
    }
  }
  
  getText(lang) {
    var text = {
      en: {id: 'ID', ucode: 'Underlying', issuer: 'Issuer', code: 'Code', wtype: 'Type', name: 'Name', lotSize: 'Lot size', bid: 'Bid', diffpt: 'Diff Point', ask: 'Ask', action: 'Action', bull: 'Bull', bear: 'Bear', put: 'Put', call: 'Call', set2: 'Set', set: 'Set', start: 'Start', pause: 'Pause', stop: 'Stop', show: 'Show', hide: 'Hide', quantity: 'Quantity', delta: 'Delta', buyin: 'Buy In', sellout: 'Sell Out', diffbid: 'Diff Bid', diffask: 'Diff Ask', lvlbid: 'Lv. Bid', wintick: 'WT', stoplost: 'SL', buyoffset: 'BO', selloffset: 'SO', lvlon: 'Level On', on: 'On', off: 'Off', rtData: 'Real Time', remove: 'Remove', netQty: 'Net Qty', turnover: 'Turnover', lastTradeTime: 'Last Trade Time', oneLvlTime: '1st Level Time', display: 'Display'},
      sc: {id: 'ID', ucode: '标的', issuer: '发行人', code: '牛熊证', wtype: '种类', name: '名称', lotSize: '手数', bid: '买入价', diffpt: '打和点', ask: '卖出价', action: '操作', bull: '牛证', bear: '熊证', put: '认沽', call: '认购', set2: '设置', set: '设置', start: '开始', pause: '暂停', stop: '停止', show: '显示', hide: '隱藏', quantity: '申购份数', delta: '对冲值', buyin: '期货买入价', sellout: '期货卖出价', diffbid: '相差买入点', sellout: '相差卖出点', lvlbid: '', wintick: '止盈', stoplost: '止蚀', buyoffset: '买入偏移', selloffset: '賣出偏移', lvlon: '打和', on: '开启', off: '停止', rtData: '即时数据', remove: '删除', netQty: '剩馀单位', turnover: '成交量', lastTradeTime: '最後交易时间', oneLvlTime: '1st 打和时间', display: '显示'},
      tc: {id: 'ID', ucode: '正股', issuer: '發行人', code: '牛熊證', wtype: '種類', name: '名稱', lotSize: '手數', bid: '買入價', diffpt: '打和點', ask: '賣出價', action: '操作', bull: '牛證', bear: '熊證', put: '認沽', call: '認購', set2: '設置', set: '設置', start: '開始', pause: '暫停', stop: '停止', show: '顯示', hide: '隐藏', quantity: '買入額', delta: '對沖值', buyin: '期貨買入價', sellout: '期貨賣出價', diffbid: '相差買入點', sellout: '相差賣出點', lvlbid: '', wintick: '止盈', stoplost: '止蝕', buyoffset: '買入偏移', selloffset: '賣出偏移', lvlon: '打和', on: '開啟', off: '停止', rtData: '即時數據', remove: '刪除', netQty: '剩餘單位', turnover: '成交量', lastTradeTime: '最後交易時間', oneLvlTime: '1st 打和時間', display: '顯示'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    
    var rows = [], no = 1
    for (var i in this.props.data) {
      var v = this.props.data[i], config = null
      // has code
      if (v.action.code.value != '' &&  v.action.code.value.length >= 5) {
        // get config
        if (this.props.data2.length >= i) {
          config = this.props.data2[i]
        }
        else {
          for (var v1 of this.props.data2) {
            if (v.action.code.value == v1.code)
              config = v1
          }
        }
        // is visible
        if (config && config.isVisable2) {
          // show
          var isShow = '', cssShow = ''
          if (config.isVisable == true)
            isShow = text.show, cssShow = 'btn-secondary'
          else if (config.isVisable == false)
            isShow = text.hide, cssShow = 'btn-light'
          
          // input
          var tv = {
            quantity: {style: null, isDisabled: true},
            delta: {style: null, isDisabled: true},
          }
          
          for (var [k, v2] of Object.entries(tv)) {
            if (v.action[k].feedback)
              tv[k].style = 'is-invalid'
            else if (v.action[k].responseResult)
              tv[k].style = 'is-valid'
          }
          
          // setting
          var tv2 = {
            wintick: {style: null, isDisabled: false},
            stoplost: {style: null, isDisabled: false},
            buyoffset: {style: null, isDisabled: false},
            selloffset: {style: null, isDisabled: false},
          }
          var btn2 = {
            set2: {style: 'btn-secondary', isDisabled: true},
            remove: {style: 'btn-secondary', isDisabled: true},
          }
          
          // lvlon
          if (parseInt(v.setting.lvlon.value) == 1)
            var lvlOn = {style: 'btn-success', isDisabled: true, text: text.on}
          else
            var lvlOn = {style: 'btn-light', isDisabled: true, text: text.off}
          
          // rtData
          if (parseInt(v.setting.rtData.value) == 1)
            var rtData = {style: 'btn-success', isDisabled: true, text: text.on}
          else
            var rtData = {style: 'btn-light', isDisabled: true, text: text.off}
          
          // btn
          var d = v.action.status
          var btn = {
            set: {style: null, isDisabled: true},
            start: {style: null, isDisabled: true},
            pause: {style: null, isDisabled: true},
            stop: {style: null, isDisabled: true},
          }
          
          if (!d.isSet===true && !d.isStart===true && !d.isPause===true && !d.isStop===true) {
            btn.set.isDisabled = false, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
            tv.quantity.isDisabled = false, tv.delta.isDisabled = false,
            btn2.set2.isDisabled = true, btn2.remove.isDisabled = false
            lvlOn.isDisabled = true, rtData.isDisabled = true
          }
          
          if (d.isSet===true) {
            btn.set.isDisabled = true, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = true,
            tv.quantity.isDisabled = true, tv.delta.isDisabled = true
            btn2.set2.isDisabled = false
            lvlOn.isDisabled = false, rtData.isDisabled = false
          }
          else if (d.isStart===true) {
            btn.set.isDisabled = true, btn.start.isDisabled = true, btn.pause.isDisabled = false, btn.stop.isDisabled = false
            tv.quantity.isDisabled = true, tv.delta.isDisabled = true
            btn2.set2.isDisabled = false
            lvlOn.isDisabled = false, rtData.isDisabled = false
          }
          else if (d.isPause===true) {
            btn.set.isDisabled = true, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = false
            tv.quantity.isDisabled = true, tv.delta.isDisabled = true
            btn2.set2.isDisabled = false
            lvlOn.isDisabled = false, rtData.isDisabled = false
          }
          else if (d.isStop===true && d.needToSetAgain==false) {
            btn.set.isDisabled = false, btn.start.isDisabled = false, btn.pause.isDisabled = true, btn.stop.isDisabled = true
            tv.quantity.isDisabled = false, tv.delta.isDisabled = false
            btn2.set2.isDisabled = false, btn2.remove.isDisabled = false
            lvlOn.isDisabled = false, rtData.isDisabled = false
          }
          else if (d.isStop===true && d.needToSetAgain==true) {
            btn.set.isDisabled = false, btn.start.isDisabled = true, btn.pause.isDisabled = true, btn.stop.isDisabled = true
            tv.quantity.isDisabled = false, tv.delta.isDisabled = false
            btn2.set2.isDisabled = false, btn2.remove.isDisabled = false
            lvlOn.isDisabled = false, rtData.isDisabled = false
          }
          
          btn.set.style = (d.isSet ? 'btn-primary' : 'btn-secondary')
          btn.start.style = (d.isStart ? 'btn-success' : 'btn-secondary')
          btn.pause.style = (d.isPause ? 'btn-warning' : 'btn-secondary')
          btn.stop.style = (d.isStop ? 'btn-danger' : 'btn-secondary')
          
          for(var k of Object.keys(tv2)) {
            var {feedback, responseResult} = v.setting[k]
            if (feedback.length > 0)
              tv2[k].style = 'is-invalid', btn2.set2.isDisabled = true
            else if (responseResult.length > 0)
              tv2[k].style = 'is-valid'
          }
          
          var btns = []
          for (const [k, v] of Object.entries(btn)) {
            btns.push(
              <button
                key={k}
                name={'is'+capitalize(k)}
                data-no={i}
                type="button"
                className={classNames('btn btn-sm mr-1', v.style)}
                disabled={v.isDisabled}
                onClick={this.handleClick}>
                  {text[k]}
              </button>
            )
          }
          
          // trade info
          var turnover = 0, lastTradeTime = '-', netQty = 0, oneLvlTime = '-'
          
          if (this.props.data4.length >= i) {
            for (var v3 of this.props.data4[i]) {
              lastTradeTime = v3.transactionTm
              
              if (v3.side == 'buy')
                netQty += parseInt(v3.matchQuantity)
              else if (v3.side == 'sell')
                netQty -= parseInt(v3.matchQuantity)
            }
          }
          
          if (this.props.data5.length >= i) {
            for (var v2 of this.props.data5[i]) {
              turnover += parseFloat(v2.sellPrice) * parseInt(v2.quantity)
              lastTradeTime = v2.soldTime
              oneLvlTime = v2.levelTime
            }
          }
          
          if (turnover==0)
            turnover = '-'
          else
            turnover = numberWithCommas(turnover)
          
          if (netQty==0)
            netQty = '-'
          else
            netQty = numberWithCommas(netQty)

          // html
          var iPriceAsk = '', iPriceBid = ''
          var cssAsk = '', cssBid = ''
          
          if (v.wPrice.ipriceAskIsWrong)
            cssAsk = "text-danger font-weight-bold"
          if (v.wPrice.ipriceBidIsWrong)
            cssBid = "text-danger font-weight-bold"
          
          if (v.wPrice.ipriceBid)
            iPriceBid = <span className={cssBid}>({v.wPrice.ipriceBid})</span> 
          if (v.wPrice.ipriceAsk)
            iPriceAsk =  <span className={cssAsk}>({v.wPrice.ipriceAsk})</span>
          
          var wtype = text[config.type]
          if (v.info.wname.includes('@') && config.type == 'bear')
            wtype = text.put
          else if (v.info.wname.includes('@') && config.type == 'bull')
            wtype = text.call
          
          // realtime data
          var cssRealTime = ''
          if (!this.props.data3.realTimeData || this.props.data3.realTimeData == false)
            cssRealTime = 'd-none'
          
          rows.push(
            <tr key={'trade_'+i}>
              <td>{no}</td>
              <td>{v.action.code.value}</td>
              <td>{v.info.issuer}</td>
              <td>{wtype}</td>
              <td>{v.info.uname}</td>
              <td>{v.action.quantity.value}</td>
              
              <td className={cssRealTime}>{v.wPrice.lvlbid}</td>
              <td className={cssRealTime}>{v.wPrice.sellout}</td>
              <td className={cssRealTime}>{v.wPrice.diffbid}</td>
              <td className={cssRealTime}>{iPriceBid} {v.wPrice.bid}</td>
              <td className={cssRealTime}>{v.wPrice.diffpt}</td>
              <td className={cssRealTime}>{v.wPrice.ask} {iPriceAsk}</td>
              <td className={cssRealTime}>{v.wPrice.buyin}</td>
              <td className={cssRealTime}>{v.wPrice.diffask}</td>
              
              <td>
              <button
                name='show'
                data-no={i}
                type="button"
                className={classNames("btn btn-sm mr-1", cssShow)}
                onClick={this.handleClick}>
                  {isShow}
              </button>
              </td>
              
              <td>
              <button
                name='lvlon'
                data-no={i}
                type="button"
                className={classNames("btn btn-sm mr-1", lvlOn.style)}
                disabled={lvlOn.isDisabled}
                onClick={this.handleClick2}>
                  {lvlOn.text}
              </button>
              </td>
              
              <td>
              <button
                name='rtData'
                data-no={i}
                type="button"
                className={classNames("btn btn-sm mr-1", rtData.style)}
                disabled={rtData.isDisabled}
                onClick={this.handleClick2}>
                  {rtData.text}
              </button>
              </td>
              
              <td>{netQty}</td>
              <td>{turnover}</td>
              <td>{lastTradeTime}</td>
              <td>{oneLvlTime}</td>
              
              <td>
              <input 
                type="text"
                name="wintick"
                className={classNames("form-control form-control-sm", tv2.wintick.style)}
                data-no={i}
                value={v.setting.wintick.value}
                disabled={tv2.wintick.isDisabled}
                onChange={this.handleChange} />
              </td>
              
              <td>
              <input 
                type="text"
                name="stoplost"
                className={classNames("form-control form-control-sm", tv2.stoplost.style)}
                data-no={i}
                value={v.setting.stoplost.value}
                disabled={tv2.stoplost.isDisabled}
                onChange={this.handleChange} />
              </td>
              
              <td>
              <input 
                type="text"
                name="buyoffset"
                className={classNames("form-control form-control-sm", tv2.buyoffset.style)}
                data-no={i}
                value={v.setting.buyoffset.value}
                disabled={tv2.buyoffset.isDisabled}
                onChange={this.handleChange} />
              </td>
              
              <td>
              <input 
                type="text"
                name="selloffset"
                className={classNames("form-control form-control-sm", tv2.selloffset.style)}
                data-no={i}
                value={v.setting.selloffset.value}
                disabled={tv2.selloffset.isDisabled}
                onChange={this.handleChange} />
              </td>
              
              <td>
              <button
                key='set2'
                name='isSet2'
                data-no={i}
                type="button"
                className={classNames('btn btn-sm mr-1', btn2.set2.style)}
                disabled={btn2.set2.isDisabled}
                onClick={this.handleClick2}>
                  {text.set2}
              </button>
              </td>
              
              <td>
              <input 
                type="text"
                name="quantity"
                className={classNames("form-control form-control-sm", tv.quantity.style)}
                data-no={i}
                value={v.action.quantity.value}
                disabled={tv.quantity.isDisabled}
                onChange={this.handleChange} />
              </td>
              
              <td>
              <input 
                type="text"
                name="delta"
                className={classNames("form-control form-control-sm", tv.delta.style)}
                data-no={i}
                value={v.action.delta.value}
                disabled={tv.delta.isDisabled}
                onChange={this.handleChange} />
              </td>
              
              <td>{btns}</td>
              
              <td>
              <button
                key='remove'
                name='remove'
                data-no={i}
                type="button"
                className={classNames('btn btn-sm mr-1', btn2.remove.style)}
                disabled={btn2.remove.isDisabled}
                onClick={this.handleClick2}>
                  {text.remove}
                </button>
              </td>
            </tr>
          )
          no++
        }
      }
    }

    return(
      <div className='row'>
      <div className="col-12 overflow-x mb-2">
      <table className="table table-sm table-striped table-light table-trade">
        <colgroup>
          <col span="1" width="20px" />
          <col span="1" width="50px" />
          <col span="1" width="50px" />
          <col span="1" width="50px" />
          <col span="1" width="120px" />
          <col span="1" width="60px" />
          
          <col span="1" width="60px" className={cssRealTime} />
          <col span="1" width="60px" className={cssRealTime} />
          <col span="1" width="60px" className={cssRealTime} />
          <col span="1" width="80px" className={cssRealTime} />
          <col span="1" width="60px" className={cssRealTime} />
          <col span="1" width="80px" className={cssRealTime} />
          <col span="1" width="60px" className={cssRealTime} />
          <col span="1" width="60px" className={cssRealTime} />
          
          <col span="1" width="60px" />
          <col span="1" width="60px" />
          <col span="1" width="60px" />
          
          <col span="1" width="60px" />
          <col span="1" width="60px" />
          <col span="1" width="120px" />
          <col span="1" width="90px" />
          
          <col span="1" width="50px" />
          <col span="1" width="50px" />
          <col span="1" width="50px" />
          <col span="1" width="50px" />
          <col span="1" width="50px" />
          
          <col span="1" width="60px" />
          <col span="1" width="60px" />
          <col span="1" width="190px" />
          <col span="1" width="60px" />
        </colgroup>
        <thead>
          <tr>
          <th>{text.id}</th>
          <th>{text.code}</th>
          <th>{text.issuer}</th>
          <th>{text.wtype}</th>
          <th>{text.ucode}</th>
          <th>{text.lotSize}</th>
          
          <th className={cssRealTime}>{text.lvlbid}</th>
          <th className={cssRealTime}>{text.sellout}</th>
          <th className={cssRealTime}>{text.diffbid}</th>
          <th className={cssRealTime}>{text.bid}</th>
          <th className={cssRealTime}>{text.diffpt}</th>
          <th className={cssRealTime}>{text.ask}</th>
          <th className={cssRealTime}>{text.buyin}</th>
          <th className={cssRealTime}>{text.diffask}</th>
          
          <th>{text.show}</th>
          <th>{text.lvlon}</th>
          <th>{text.rtData}</th>
          
          <th>{text.netQty}</th>
          <th>{text.turnover}</th>
          <th>{text.lastTradeTime}</th>
          <th>{text.oneLvlTime}</th>
          
          <th>{text.wintick}</th>
          <th>{text.stoplost}</th>
          <th>{text.buyoffset}</th>
          <th>{text.selloffset}</th>
          <th>{text.action}</th>
          
          <th>{text.quantity}</th>
          <th>{text.delta}</th>
          <th>{text.action}</th>
          <th>{text.display}</th>
          
          </tr>
        </thead>
        <tbody>
          {rows}
        </tbody>
      </table>
      </div>
      </div>
    )
  }
}