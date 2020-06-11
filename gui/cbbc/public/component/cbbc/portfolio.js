class Portfolio extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.portfolio = {}
  }
  
  static getDerivedStateFromProps(props, state) {
    if (typeof props.data !== 'undefined' && props.data.length > 0) {
      for (var arr of props.data) {
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
          // 牛熊证，总买入价，总卖出价, 总买入单位, 总盈亏, 最后卖出时时
          for (const [code, datas1] of Object.entries(datas)) {
            var noExecution=0, totalBuyPrice=0, totalSellPrice=0, totalQuantity=0, totalProfitLoss=0, lastSoldTime=null, totalWin=0, totalLoss=0, totalDraw=0
            for (var item1 of datas1) {
              noExecution+=1
              totalBuyPrice+=item1.buyPrice
              totalSellPrice+=item1.sellPrice
              totalQuantity+=item1.quantity
              totalProfitLoss+=item1.profitLoss
              lastSoldTime=item1.soldTime
              totalWin = (item1.sellPrice>item1.buyPrice) ? totalWin+1 : totalWin
              totalLoss = (item1.sellPrice<item1.buyPrice) ? totalLoss+1 : totalLoss
              totalDraw = (item1.sellPrice==item1.buyPrice) ? totalDraw+1 : totalDraw
            }
            // 平均
            state.portfolio[code] = {
              avgBuyPrice: totalBuyPrice/noExecution,
              avgSellPrice: totalSellPrice/noExecution,
              totalQuantity: totalQuantity,
              totalProfitLoss: totalProfitLoss,
              noExecution: noExecution,
              lastSoldTime: lastSoldTime,
              totalWin: totalWin,
              totalLoss: totalLoss,
              totalDraw: totalDraw
            }
          }
        }
      }
    }
    return state
  }
  
  getText(lang) {
    var text = {
      en: {
        id: 'ID', portfolio: 'Portfolio', buyPrice: 'Avg Buy Price', code: 'Code', quantity: 'Total Quantity', ref: 'Ref', sellPrice: 'Avg Sell Price', soldTime: 'Last Update Time', profitLoss : 'Total P&L', noExecution: 'No. Execution', totalWin: 'Win', totalLoss: 'Loss', totalDraw: 'Draw'
      },
      sc: {
        id: 'ID', portfolio: '明细表', buyPrice: '平均买入价', code: '牛能证', quantity: '总买入单位', ref: 'Ref', sellPrice: '平均卖出价', soldTime: '最后卖出时间', profitLoss : '总盈亏', noExecution: '执行次数', totalWin: '赢', totalLoss: '亏', totalDraw: '平'
      },
      tc: {
        id: 'ID', portfolio: '賺蝕紀錄', buyPrice: '平均買入價', code: '牛能證', quantity: '總買入單位', ref: 'Ref', sellPrice: '平均賣出價', soldTime: '最後賣出時間', profitLoss : '總盈虧', noExecution: '執行次數', totalWin: '贏', totalLoss: '虧', totalDraw: '平'
      }
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var rows = []
    var no = 0
    var len = Object.keys(this.state.portfolio).length
    for (const [code, d] of Object.entries(this.state.portfolio)) {
      var style = (d.totalProfitLoss==0) ? '' : (d.totalProfitLoss>0) ? 'font-up' : 'font-down'
      rows.push(
        <tr key={'portfolio_'+no}>
          <td>{len-no}</td>
          <td>{code}</td>
          <td>{ parseFloat(d.avgBuyPrice).toFixed(4) }</td>
          <td>{ parseFloat(d.avgSellPrice).toFixed(4) }</td>
          <td>{ numberWithCommas(d.totalQuantity) }</td>
          <td className={style}>{ numberWithCommas(parseFloat(d.totalProfitLoss).toFixed(2)) }</td>
          <td>{d.totalWin}</td>
          <td>{d.totalLoss}</td>
          <td>{d.totalDraw}</td>
          <td>{d.noExecution}</td>
          <td>{d.lastSoldTime}</td>
        </tr>
      )
      no+=1
    }
    
    return(
      <div className="row">
        <div className="col-12">
          <h6> {text.portfolio} </h6>
          <table className="table table-sm table-striped table-light table-portfolio">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
              <col span="1" width="50px" />
              <col span="1" width="50px" />
              <col span="1" width="50px" />
              <col span="1" width="150px" />
              <col span="1" width="150px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.code}</th>
                <th>{text.buyPrice}</th>
                <th>{text.sellPrice}</th>
                <th>{text.quantity}</th>
                <th>{text.profitLoss}</th>
                <th>{text.totalWin}</th>
                <th>{text.totalLoss}</th>
                <th>{text.totalDraw}</th>
                <th>{text.noExecution}</th>
                <th>{text.soldTime}</th>
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