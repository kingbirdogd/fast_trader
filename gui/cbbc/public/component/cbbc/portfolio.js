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
    this.handleClick = this.handleClick.bind(this)
  }
  
  componentDidMount() {
    $('[data-toggle="tooltip"]').tooltip()
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
            var noExecution=0, totalBuyPrice=0, totalSellPrice=0, totalQuantity=0, totalProfitLoss=0, lastSoldTime=null, totalWin=0, totalLoss=0, totalDraw=0, issuer=null, wtype=null, levelTimeWin=[0, 0, 0], levelTimeLoss=[0, 0, 0], wintime=[0,0,0], winTick=[0,0,0], lossTick=[0,0,0], lvlCount=[0,0,0], underlying=null
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
              issuer = item1.issuer
              wtype = item1.wtype
              underlying = item1.underlying
              
              var lvlTime = Math.abs(item1.levelTime)
              var _wintime = Math.abs(item1.wintime)
              var tick = Math.abs((item1.sellPrice-item1.buyPrice)/0.001)
              
              // win + break even
              if (item1.sellPrice >= item1.buyPrice) {
                // only win
                if (item1.sellPrice > item1.buyPrice) {
                  // lvl time
                  if (lvlTime > 0 && lvlTime <= 1)
                    levelTimeWin[0] += 1
                  else if (lvlTime > 1 && lvlTime <= 2)
                    levelTimeWin[1] += 1
                  else if (lvlTime >= 3)
                    levelTimeWin[2] += 1

                  // win time
                  if (_wintime>=0 && _wintime<=1)
                    wintime[0] += 1
                  else if (_wintime>1 && _wintime<=2)
                    wintime[1] += 1
                  else if (_wintime>=3)
                    wintime[2] += 1
                }
                
                // win tick
                if (tick>0 && tick <=1)
                  winTick[0] += 1
                else if (tick>1 && tick <=2)
                  winTick[1] += 1
                else if (tick>=3)
                  winTick[2] += 1
              }
              // loss
              else if (item1.sellPrice < item1.buyPrice) {
                if (lvlTime > 0 && lvlTime <= 1)
                  levelTimeLoss[0] += 1
                else if (lvlTime > 1 && lvlTime <= 2)
                  levelTimeLoss[1] += 1
                else if (lvlTime >= 3)
                  levelTimeLoss[2] += 1
                
                // loss tick
                if (tick>0 && tick <=1)
                  lossTick[0] += 1
                else if (tick>1 && tick <=2)
                  lossTick[1] += 1
                else if (tick>=3)
                  lossTick[2] += 1
              }
              
              // lvlCount
              if (item1.lvlcount<=0)
                lvlCount[0] += 1
              else if (item1.lvlcount>1 && item1.lvlcount <=2)
                lvlCount[1] += 1
              else if (item1.lvlcount>=3)
                lvlCount[2] += 1
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
              totalDraw: totalDraw,
              issuer: issuer,
              wtype: wtype,
              levelTimeWin: levelTimeWin.join(', '),
              levelTimeLoss: levelTimeLoss.join(', '),
              wintime: wintime.join(', '),
              winTick: winTick.join(', '),
              lossTick: lossTick.join(', '),
              lvlCount: lvlCount.join(', '),
              underlying: underlying,
            }
          }
        }
      }
    }
    return state
  }
  
  handleClick() {
    event.preventDefault()
    
    var tb = $('.table-portfolio'),
        data = []
        
    tb.find('tr').each(function() {
      var row = []
      // header
      $(this).find('th').each(function() {
        if ($(this).html().includes('span'))
          row.push($(this).children().html().replaceAll(',', ''))
        else
          row.push($(this).html().replaceAll(',', ''))
      })
      // content
      $(this).find('td').each(function() {
        row.push($(this).html().replaceAll(',', ''))
      })
      data.push(row)
    })
    
    var url = 'https://chart.dbpower.com.hk/PHPExcel/download.php'
    var title = 'portfolio'
    var name = title+'_'+moment().format('YYYYMMDD')
    var data = JSON.stringify(data).replaceAll('"', '').replaceAll('[[', '').replaceAll(']]', '').replaceAll('],[', '|')
    fakeFormSubmit(url, {name: name, title: title, data: data})
  }
  
  getText(lang) {
    var text = {
      en: {
        id: 'ID', portfolio: 'Portfolio', buyPrice: 'Avg Buy Price', code: 'Code', quantity: 'Total Quantity', ref: 'Ref', sellPrice: 'Avg Sell Price', soldTime: 'Last Update Time', profitLoss : 'Total P&L', noExecution: 'No. Exec', totalWin: 'Win', totalLoss: 'Loss', totalDraw: 'Draw', wtype: 'Type', issuer: 'Issuer', levelTime: 'Lvl Time', save: 'Export', breakEven: 'Break Even', winTick: 'Win Tick', lossTick: 'Loss Tick', winTime: 'Win Time', lossTime: 'Loss Time', lvlCount: 'Lvl Count', download: 'Download', underlying: 'Underlying'
      },
      sc: {
        id: 'ID', portfolio: '明细表', buyPrice: '平均买入价', code: '牛能证', quantity: '总买入单位', ref: 'Ref', sellPrice: '平均卖出价', soldTime: '最后卖出时间', profitLoss : '总盈亏', noExecution: '执行次数', totalWin: '赢', totalLoss: '亏', totalDraw: '平', wtype: '种类', issuer: '发行人', levelTime: '打和时间', save: '汇出', breakEven: '打和', winTick: '盈利格数', lossTick: '亏蚀格數', winTime: '嬴利时间', lossTime: '亏蚀時間', lvlCount: '买卖价差', download: '下载', underlying: '标的'
      },
      tc: {
        id: 'ID', portfolio: '賺蝕紀錄', buyPrice: '平均買入價', code: '牛能證', quantity: '總買入單位', ref: 'Ref', sellPrice: '平均賣出價', soldTime: '最後賣出時間', profitLoss : '總盈虧', noExecution: '執行次數', totalWin: '贏', totalLoss: '虧', totalDraw: '平', wtype: '種類', issuer: '發行人', levelTime: '打和時間', save: '匯出', breakEven: '打和', winTick: '盈利格數', lossTick: '虧蝕格數', winTime: '嬴利時間', lossTime: '亏蚀時間', lvlCount: '買賣價差', download: '下載', underlying: '正股'
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
          <td>{d.underlying}</td>
          <td>{d.issuer}</td>
          <td>{d.wtype}</td>
          <td>{ parseFloat(d.avgBuyPrice).toFixed(4) }</td>
          <td>{ parseFloat(d.avgSellPrice).toFixed(4) }</td>
          <td>{ numberWithCommas(d.totalQuantity) }</td>
          <td className={style}>{ numberWithCommas(parseFloat(d.totalProfitLoss).toFixed(2)) }</td>
          
          <td>{d.totalWin}</td>
          <td>{d.totalLoss}</td>
          <td>{d.totalDraw}</td>
          <td>{d.noExecution}</td>
          
          <td>{d.winTick}</td>
          <td>{d.lossTick}</td>
          
          <td>{d.levelTimeWin}</td>
          <td>{d.levelTimeLoss}</td>
          <td>{d.wintime}</td>
          <td className="d-none">{d.lvlCount}</td>
          
          <td>{d.lastSoldTime}</td>
        </tr>
      )
      no+=1
    }
    
    return(
      <div className="row">
        <div className="col-12">
          <div className="row">
            <div className="col-6"> <h6> {text.portfolio} </h6> </div>
            <div className="col-6"> <button className="btn btn-sm btn-secondary float-right" onClick={this.handleClick}>{text.download} {text.portfolio}</button> </div>
          </div>
          
          <table className="table table-sm table-striped table-light table-portfolio">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="50px" />
              <col span="1" width="100px" />
              <col span="1" width="50px" />
              <col span="1" width="50px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              
              <col span="1" width="60px" />
              <col span="1" width="60px" />
              <col span="1" width="60px" />
              <col span="1" width="60px" />
              
              <col span="1" width="80px" />
              <col span="1" width="80px" />
              
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col span="1" width="100px" />
              <col className="d-none" span="1" width="100px" />
              
              <col span="1" width="150px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.code}</th>
                <th>{text.underlying}</th>
                <th>{text.issuer}</th>
                <th>{text.wtype}</th>
                <th>{text.buyPrice}</th>
                <th>{text.sellPrice}</th>
                <th>{text.quantity}</th>
                <th>{text.profitLoss}</th>
                
                <th>{text.totalWin}</th>
                <th>{text.totalLoss}</th>
                <th>{text.totalDraw}</th>
                <th>{text.noExecution}</th>
                
                <th><span data-toggle="tooltip" data-placement="top" title="<1 tick | 1~2 ticks | >3 ticks"> {text.winTick}</span></th>
                <th><span data-toggle="tooltip" data-placement="top" title="<1 tick | 1~2 ticks | >3 ticks"> {text.lossTick}</span></th>
                
                <th><span data-toggle="tooltip" data-placement="top" title="<1s | 1~2s | >3s"> {text.levelTime} ({text.totalWin})</span></th>
                <th><span data-toggle="tooltip" data-placement="top" title="<1s | 1~2s | >3s"> {text.levelTime} ({text.totalLoss})</span></th>
                <th><span data-toggle="tooltip" data-placement="top" title="<1s | 1~2s | >3s"> {text.winTime}</span></th>
                <th className="d-none"><span data-toggle="tooltip" data-placement="top" title="<1 tick | 1~2 ticks | >3 ticks"> {text.lvlCount}</span></th>
                
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