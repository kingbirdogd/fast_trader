class Position extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    data2: PropTypes.object,
    data3: PropTypes.object,
    data4: PropTypes.object,
    data5: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.position = {}
    this.handleForceSell = this.handleForceSell.bind(this)
    this.handleWs = this.handleWs.bind(this)
    this.handleWls = this.handleWls.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    var datas = {}
    if (typeof props.data !== 'undefined' && props.data.length > 0) {
      for (var arr of props.data) {
        if (typeof arr !== 'undefined' && arr.length > 0) {
          // code主键
          var codes = []
          for (var item of arr)
            codes.push(item.code)
          codes = Array.from(new Set(codes))
          // 重构数据集
          for (var code of codes)
            datas[code] = []
          // 注入数据
          for (var item of arr)
            datas[item.code].push(item)
        }
      }
    }
    
    // 牛熊证，买入价，卖出价，买入单位, 卖出单位， 买入次数
    for (const [code, datas1] of Object.entries(datas)) {
      var buyTotalQuantity=0, buyTotalPrice=0, buyCount=0, buyQuantity=0, buyPrice=[]
      var sellTotalQuantity=0, sellTotalPrice=0, sellCount=0, sellQuantity=0, sellPrice=0
      var wbid=0, stoplost=0, pnl=0
      var ucode=0
      for (var item1 of datas1) {
        if (item1.side=='buy') {
          buyTotalPrice += item1.matchPrice
          buyTotalQuantity += item1.matchQuantity
          buyCount += 1
          buyPrice.push(item1.matchPrice)
          buyQuantity = item1.matchQuantity
          ucode = item1.ucode
          wbid = item1.wbid
          stoplost = (item1.ucode in props.data4) ? props.data4[item1.ucode].stoplost : item1.stoplost
        }
        else if (item1.side=='sell') {
          sellTotalPrice += item1.matchPrice
          sellTotalQuantity += item1.matchQuantity
          sellCount += 1
          sellPrice = item1.matchPrice
          sellQuantity = item1.matchQuantity
          ucode = item1.ucode
        }
        // 手中持有倉位從其他渠道賣出
        else if (item1.side=='reset') {
          var remainQuantity = buyTotalQuantity-sellTotalQuantity
          // 如手中持有，新增一筆賣盤，以抵銷之前的倉位
          if (remainQuantity>0) {
            sellTotalPrice += item1.matchPrice
            sellTotalQuantity += remainQuantity
            sellCount += 1
            sellPrice = item1.matchPrice
            sellQuantity = item1.remainQuantity
            ucode = item1.ucode
          }
        }
        
        // 已平倉
        if (buyTotalQuantity-sellTotalQuantity<=0) {
          buyQuantity=0, buyPrice=[], sellQuantity=0, sellPrice=0
          delete state.position[code]
        }
        // 未平倉
        else if (buyTotalQuantity-sellTotalQuantity>0 && buyQuantity>0) {
          // 蟹左半倉
          if (buyQuantity>0 && sellQuantity>0) {}
          // 未扔貨
          else if(buyQuantity>0 && sellQuantity<=0) {}
          // 平均买入价
          var bpSum = buyPrice.reduce((a, b) => a + b, 0)
          var bpAvg = (bpSum / buyPrice.length) || 0
          
          var pnl = wbid-bpAvg
          if ((code in props.data2) && props.data2[code].bid > 0) {
            // 輪 bid價
            wbid = props.data2[code].bid
            // 盈亏
            pnl = wbid-bpAvg
          }
          
          state.position[code] = {
            buyPrice: bpAvg,
            buyQuantity: buyTotalQuantity-sellTotalQuantity,
            sellPrice: sellPrice,
            sellQuantity: sellQuantity,
            transactionTm: item1.transactionTm,
            buyCount: buyCount,
            sellCount: sellCount,
            ucode: ucode,
            wbid: wbid,
            stoplost: stoplost,
            pnl: pnl
          }
        }
      }
    }
    
    return state
  }
  
  handleForceSell() {
    var code = parseInt(event.target.name)
    var ucode = parseInt(this.state.position[code].ucode)
    var price = parseFloat(this.state.position[code].wbid)
    
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var issuer = states.issuer.curIssuer
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var command = {cmd: 'force_sell', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), code: code, ucode: ucode, price: formatLongV2(price)}
    if (price>0)
      sendWebsocket(JSON.stringify(command))
  }
  
  switchAction(action) {
    if (action == 'set')
      return 'unset'
    else if (action == 'unset')
      return 'set'
    else
      return null
  }
  
  handleWs() {
    var code = parseInt(event.target.name)
    var ucode = parseInt(this.state.position[code].ucode)
    var actions = this.props.data5
    
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var action = 'set'
    if ((code in actions) && 'winsell' in actions[code])
      action = this.switchAction(actions[code].winsell.setaction)
    
    var command = {cmd: 'winsell', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), code: code, ucode: ucode, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  handleWls() {
    var code = parseInt(event.target.name)
    var ucode = parseInt(this.state.position[code].ucode)
    var actions = this.props.data5
    
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var action = 'set'
    if ((code in actions) && 'winlvlsell' in actions[code])
      action = this.switchAction(actions[code].winlvlsell.setaction)
    
    var command = {cmd: 'winlvlsell', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), code: code, ucode: ucode, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {position: 'Position', ucode: 'Underlying', buyPrice: 'Buy Price', buyQuantity: 'Buy Quantity', sellPrice: 'Sell Price', 
            sellQuantity: 'Sell Quantity', id: 'ID', code: 'Code', transactionTm: 'Last Update Time', warrantPrice: 'Wnt Price', stopLost: 'Stop Lost', pnl: 'Gain', forceSell: 'Sell', ws: 'WS', wls: 'WLS'},
      sc: {position: '持仓', ucode: '相关资产', buyPrice: '买入价', buyQuantity: '买入单位', sellPrice: '卖出价', 
            sellQuantity: '卖出单位', id: 'ID', code: '牛熊证', transactionTm: '交易时间', warrantPrice: '轮证现价', stopLost: '止损价', pnl: '盈亏', forceSell: '卖出', ws: '赚/卖', wls: '赚/平/卖'},
      tc: {position: '持倉', ucode: '相關資產', buyPrice: '買入價', buyQuantity: '買入單位', sellPrice: '賣出價', 
            sellQuantity: '賣出單位', id: 'ID', code: '牛熊證', transactionTm: '交易時間', warrantPrice: '輪證現價', stopLost: '止蝕價', pnl: '盈虧', forceSell: '賣出', ws: '賺/賣', wls: '賺/平/賣'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var len = Object.keys(this.state.position).length
    var rows = []
    var no = 0
    // 排序
    var orderKeys1 = Object.keys(this.state.position).reverse()
    for (var code of orderKeys1) {
      var d = this.state.position[code]
      var style = (d.pnl==0) ? '' : (d.pnl>0) ? 'font-up' : 'font-down'
      var ucode = (d.ucode>0) 
        ? d.ucode 
        : ((this.props.data3) && (code in this.props.data3)) 
        ? this.props.data3[code] 
        : ''
      var uname = getUnderlyingName2(ucode)
      
      // 按钮
      var btn1Stype = '', btn1IsDisable = false
      var btn2Stype = '', btn2IsDisable = false
      if (code in this.props.data5) {
        for (const [k, v] of Object.entries(this.props.data5[code])) {
          if (k == 'winsell' && v.result == 'success' && v.setaction == 'set')
            btn1Stype = 'btn-success'
          else if (k == 'winsell' && v.result == 'success' && v.setaction == 'unset')
            btn1Stype = 'btn-secondary'
          else if (k == 'winsell' && v.result == 'fail')
            btn1Stype = 'btn-danger'
          
          else if (k == 'winlvlsell'&& v.result == 'success' && v.setaction == 'set')
            btn2Stype = 'btn-success'
          else if (k == 'winlvlsell' && v.result == 'success' && v.setaction == 'unset')
            btn2Stype = 'btn-secondary'
          else if (k == 'winlvlsell' && v.result == 'fail')
            btn2Stype = 'btn-danger'
        }
      }
      
      rows.push(
        <tr key={'position_'+no}>
          <td>{no+1}</td>
          <td>{code}</td>
          <td>{ucode} {uname}</td>
          <td>{ parseFloat(d.buyPrice).toFixed(4) }</td>
          <td> { parseFloat(d.wbid).toFixed(3)} </td>
          <td>{ parseFloat(d.sellPrice).toFixed(4)}</td>
          <td> { parseFloat(d.stoplost).toFixed(2)} </td>
          <td>{ numberWithCommas(d.buyQuantity) }</td>
          <td className={style}>{ parseFloat(d.pnl).toFixed(4) }</td>
          <td>{ numberWithCommas(d.sellQuantity) }</td>
          <td>{d.transactionTm}</td>
          <td>
          
            <button
              name={code}
              type="button"
              className="btn btn-sm btn-secondary"
              onClick={this.handleForceSell}>
                {text.forceSell}
            </button>
            
            <button
              name={code}
              type="button"
              className={classNames("btn btn-sm btn-secondary", btn1Stype)}
              disabled={btn1IsDisable}
              onClick={this.handleWs}>
                {text.ws}
            </button>
            
            <button
              name={code}
              type="button"
              className={classNames("btn btn-sm btn-secondary", btn2Stype)}
              disabled={btn2IsDisable}
              onClick={this.handleWls}>
                {text.wls}
            </button>
            
          </td>
        </tr>)
      no+=1
    }
    
    return(
      <div className='row'>
        <div className="col-12">
        <h6> {text.position} </h6>
          <table className="table table-sm table-striped table-light table-position">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="100px" />
              <col span="1" width="200px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="200px" />
              <col span="1" width="180px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.code}</th>
                <th>{text.ucode}</th>
                <th>{text.buyPrice}</th>
                <th>{text.warrantPrice}</th>
                <th>{text.sellPrice}</th>
                <th>{text.stopLost}</th>
                <th>{text.buyQuantity}</th>
                <th>{text.pnl}</th>
                <th>{text.sellQuantity}</th>
                <th>{text.transactionTm}</th>
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