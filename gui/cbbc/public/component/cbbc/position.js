class Position extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    data2: PropTypes.array,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.position = {}
    this.handleForceSell = this.handleForceSell.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    if (typeof props.data !== 'undefined' && props.data.length > 0) {
      for (var i in props.data) {
        var arr = props.data[i]
        if (typeof arr !== 'undefined' && arr.length > 0) {
          // code主键
          var codes = []
          for (var item of arr)
            codes.push(item.code)
          codes = Array.from(new Set(codes))
          // 重构数据集
          var datas = {}
          for (var code of codes)
            datas[code] = []
          // 注入数据
          for (var item of arr)
            datas[item.code].push(item)
          // 牛熊证，买入价，卖出价，买入单位, 卖出单位， 买入次数
          for (const [code, datas1] of Object.entries(datas)) {
            var buyTotalQuantity=0, buyQuantity=0, buyTurnover=[]
            var sellTotalQuantity=0, sellQuantity=0, sellPrice=0
            var wbid = 0, pnl = 0
            
            for (var item1 of datas1) {
              if (item1.side=='buy') {
                buyTotalQuantity += item1.matchQuantity
                buyTurnover.push(item1.matchPrice*item1.matchQuantity)
                buyQuantity = item1.matchQuantity
              }
              else if (item1.side=='sell') {
                sellTotalQuantity += item1.matchQuantity
                sellPrice = item1.matchPrice
                sellQuantity = item1.matchQuantity
              }
              // 手中持有倉位從其他渠道賣出
              else if (item1.side=='reset') {
                var remainQuantity = buyTotalQuantity-sellTotalQuantity
                // 如手中持有，新增一筆賣盤，以抵銷之前的倉位
                if (remainQuantity>0) {
                  sellTotalQuantity += remainQuantity
                  sellPrice = item1.matchPrice
                  sellQuantity = item1.remainQuantity
                }
              }

              // 已平倉
              if (buyTotalQuantity-sellTotalQuantity<=0) {
                buyTotalQuantity=0, buyQuantity=0, buyTurnover=[]
                sellTotalQuantity=0, sellQuantity=0, sellPrice=0
                delete state.position[code]
              }
              // 未平倉
              else if (buyTotalQuantity-sellTotalQuantity>0 && buyQuantity>0) {
                // 蟹左半倉
                if (buyQuantity>0 && sellQuantity>0) {}
                // 未扔貨
                else if(buyQuantity>0 && sellQuantity<=0) {}
                // 平均买入价
                var bpSum = buyTurnover.reduce((a, b) => a + b, 0)
                var bpAvg = (bpSum / buyTotalQuantity) || 0
                
                if (props.data2.length >= i && props.data2[i].bid > 0) {
                  // 輪 bid價
                  wbid = props.data2[i].bid
                  // 盈亏
                  pnl = wbid-bpAvg
                }
                
                state.position[code] = {
                  buyPrice: bpAvg,
                  buyQuantity: buyTotalQuantity-sellTotalQuantity,
                  sellPrice: sellPrice,
                  sellQuantity: sellQuantity,
                  transactionTm: item1.transactionTm,
                  pnl: pnl,
                  wbid: wbid
                }
              }
            }
          }
        }
      }
    }
    return state
  }
  
  handleForceSell() {
    var no = event.target.attributes.getNamedItem('data-no').value
    var code = parseInt(event.target.name)
    var price = parseFloat(this.state.position[code].wbid)
    var buyQuantity = parseFloat(this.state.position[code].buyQuantity)
    var states = this.props.getStates()
    
    var command = {
      cmd: 'force_sell',
      warrant_code: parseInt(code),
      price: formatLongV2(price),
      quantity: formatLongV2(buyQuantity),
      ref: states.prefix+no,
      algo_name: states.modules[states.config.value[no]],
      id: parseInt(states.userId)
    }
    if (price>0)
      sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {position: 'Position', buyPrice: 'Buy Price', buyQuantity: 'Buy Quantity', sellPrice: 'Sell Price', 
            sellQuantity: 'Sell Quantity', id: 'ID', code: 'Code', transactionTm: 'Last Update Time', pnl: 'Gain', forceSell: 'Sell', bidPrice:'Wnt Price'},
      sc: {position: '持仓', buyPrice: '买入价', buyQuantity: '买入单位', sellPrice: '卖出价', 
            sellQuantity: '卖出单位', id: 'ID', code: '牛熊证', transactionTm: '交易时间', pnl: '盈亏', forceSell: '卖出', bidPrice: '轮证卖出价'},
      tc: {position: '持倉', buyPrice: '買入價', buyQuantity: '買入單位', sellPrice: '賣出價', 
            sellQuantity: '賣出單位', id: 'ID', code: '牛熊證', transactionTm: '交易時間', pnl: '盈虧', forceSell: '賣出', bidPrice: '輪證賣出價'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var len = Object.keys(this.state.position).length
    var rows = []
    var no = 0
    for (const [code, d] of Object.entries(this.state.position)) {
      var style = (d.pnl==0) ? '' : (d.pnl>0) ? 'font-up' : 'font-down'
      var idx = no
      if (code in this.props.data3)
        idx = this.props.data3[code]
      rows.push(
        <tr key={'position_'+no}>
          <td>{len-no}</td>
          <td>{code}</td>
          <td>{ parseFloat(d.buyPrice).toFixed(4) }</td>
          <td>{ parseFloat(d.wbid).toFixed(4) }</td>
          <td>{ parseFloat(d.sellPrice).toFixed(4) }</td>
          <td>{ numberWithCommas(d.buyQuantity) }</td>
          <td>{ numberWithCommas(d.sellQuantity) }</td>
          <td className={style}>{ parseFloat(d.pnl).toFixed(4) }</td>
          <td>{d.transactionTm}</td>
          <td>
            <button
              name={code}
              data-no={idx}
              type="button"
              className="btn btn-sm btn-secondary"
              onClick={this.handleForceSell}>
                {text.forceSell}
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
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.code}</th>
                <th>{text.buyPrice}</th>
                <th>{text.bidPrice}</th>
                <th>{text.sellPrice}</th>
                <th>{text.buyQuantity}</th>
                <th>{text.sellQuantity}</th>
                <th>{text.pnl}</th>
                <th>{text.transactionTm}</th>
                <th></th>
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