class Portfolio extends React.Component {
  static propTypes = {
    data: PropTypes.array,
    data2: PropTypes.object,
    data3: PropTypes.object,
    data4: PropTypes.object,
    data5: PropTypes.string,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.state.portfolio = {}
    this.handleAction = this.handleAction.bind(this)
    this.handleAction2 = this.handleAction2.bind(this)
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
    
    // 牛熊证，总买入价，总卖出价, 总买入单位, 总盈亏, 最后卖出时間
    for (const [code, datas1] of Object.entries(datas)) {
      var noExecution=0, totalBuyAmount=0, totalSellAmount=0, totalQuantity=0, totalProfitLoss=0, lastSoldTime=null, totalWin=0, totalLoss=0, totalDraw=0, ucode=null, issuer=null
      for (var item1 of datas1) {
        noExecution+=1
        totalBuyAmount+=item1.buyPrice*item1.quantity
        totalSellAmount+=item1.sellPrice*item1.quantity
        totalQuantity+=item1.quantity
        totalProfitLoss+=item1.profitLoss
        lastSoldTime=item1.soldTime
        totalWin = (item1.sellPrice>item1.buyPrice) ? totalWin+1 : totalWin
        totalLoss = (item1.sellPrice<item1.buyPrice) ? totalLoss+1 : totalLoss
        totalDraw = (item1.sellPrice==item1.buyPrice) ? totalDraw+1 : totalDraw
        ucode = (code in props.data2) ? props.data2[code] : ''
        issuer = item1.issuer
      }
      // 平均
      state.portfolio[code] = {
        avgBuyPrice: totalBuyAmount/totalQuantity,
        avgSellPrice: totalSellAmount/totalQuantity,
        totalBuyAmount: totalBuyAmount,
        totalSellAmount: totalSellAmount,
        totalQuantity: totalQuantity,
        totalProfitLoss: totalProfitLoss,
        noExecution: noExecution,
        lastSoldTime: lastSoldTime,
        totalWin: totalWin,
        totalLoss: totalLoss,
        totalDraw: totalDraw,
        ucode: ucode,
        issuer: issuer
      }
    }
    
    return state
  }
  
  handleAction() {
    // 傳入數據
    var code = parseInt(event.target.name)
    var ucode = parseInt(this.props.data2[code])
    
    // 更新狀態
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states.wntList2)
    if (!(code in obj)) {
      var obj2 = {}
      obj2.curState = null
      obj2.feedback = null
      obj2.responseResult = null
      obj[code] = obj2
      this.props.setStates({wntList2: obj})
    }
    
    // 傳送指令
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var data3 = this.props.data3
    if (!(code in data3) || data3[code].curState == null || typeof data3[code].curState == 'undefined')
      var actionNext = 'remove'
    else if (data3[code].curState == 'select')
      var actionNext = 'remove'
    else if (data3[code].curState == 'remove')
      var actionNext = 'select'
    
    var command = {cmd: 'selectwarrant', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), code: code, action: actionNext}
    sendWebsocket(JSON.stringify(command))
  }
  
  handleAction2() {
    var action = event.target.getAttribute('data-action')
    var ucode = parseInt(event.target.name)
    
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var command = {cmd: 'selectunderlying', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), ucode: ucode, action: action}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {
        id: 'ID', portfolio: 'Portfolio', ucode: 'Underlying', buyPrice: 'Avg Buy Price', code: 'Code', quantity: 'Total Quantity', ref: 'Ref', sellPrice: 'Avg Sell Price', soldTime: 'Last Update Time', profitLoss : 'P&L', noExecution: 'No. Execution', totalWin: 'Win', totalLoss: 'Loss', totalDraw: 'Draw', totalProfitLoss: 'Total P&L', issuer: 'Issuer', select: 'Select', remove: 'Remove', error: 'Error'
      },
      sc: {
        id: 'ID', portfolio: '明细表', ucode: '相关资产', buyPrice: '平均买入价', code: '牛能证', quantity: '总买入单位', ref: 'Ref', sellPrice: '平均卖出价', soldTime: '最后卖出时间', profitLoss : '盈亏', noExecution: '执行次数', totalWin: '赢', totalLoss: '亏', totalDraw: '平', totalProfitLoss: '总盈亏', issuer: '發行人', select: '选择', remove: '剔除', error: '错误'
      },
      tc: {
        id: 'ID', portfolio: '賺蝕紀錄', ucode: '相關資產', buyPrice: '平均買入價', code: '牛能證', quantity: '總買入單位', ref: 'Ref', sellPrice: '平均賣出價', soldTime: '最後賣出時間', profitLoss : '盈虧', noExecution: '執行次數', totalWin: '贏', totalLoss: '虧', totalDraw: '平', totalProfitLoss: '總盈虧', issuer: '發行人', select: '選擇', remove: '剔除', error: '錯誤'
      }
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var rows = []
    var no = 0
    var len = Object.keys(this.state.portfolio).length
    var totalProfitLoss1 = 0
    for (const [code, d] of Object.entries(this.state.portfolio)) {
      d.ucode = formatCode(d.ucode,5)
      
      var style = (d.totalProfitLoss==0) ? '' : (d.totalProfitLoss>0) ? 'font-up' : 'font-down'
      var uname = getUnderlyingName2(d.ucode)
      totalProfitLoss1 += d.totalProfitLoss
      
      // 按鈕1
      if (!(code in this.props.data3))
        var btn1Stype = null, btn1Text = text.remove
      // 成功
      else if (code in this.props.data3 && this.props.data3[code].curState=='remove' && this.props.data3[code].responseResult)
        var btn1Stype = 'btn-danger', btn1Text = text.remove
      else if (code in this.props.data3 && this.props.data3[code].curState=='select' && this.props.data3[code].responseResult)
        var btn1Stype = 'btn-success', btn1Text = text.select
      // 錯誤
      else if (code in this.props.data3 && this.props.data3[code].feedback)
        var btn1Stype = 'btn-warning', btn1Text = text.error
      
      // 按鈕2
      var {removed, selected} = this.props.data4
      // 預設
      if (!(removed.includes(d.ucode)) && !(selected.includes(d.ucode))) {
        // remove
        if (this.props.data5=='remove')
          var btn2Stype = null, btn2Text = text.remove, btn2Action = 'remove'
        // select
        else if (this.props.data5=='select')
          var btn2Stype = null, btn2Text = text.select, btn2Action = 'select'
      }
      else if (removed.includes(d.ucode)) {
        var btn2Stype = 'btn-danger', btn2Text = text.remove, btn2Action = 'select'
      }
      else if (selected.includes(d.ucode)) {
        var btn2Stype = 'btn-success', btn2Text = text.select, btn2Action = 'remove'
      }
      
      
      // 顯示隱藏按鈕1
      var isShowWarrantSelector = {display: 'none'}, modules = this.props.getStates().modules
      if (('call' in modules) && modules.call && modules.call.includes('s1') || 
          ('put' in modules) && modules.put && modules.put.includes('s1')) {
        isShowWarrantSelector = {display: ''}
      }
      
      rows.push(
        <tr key={'portfolio_'+no}>
          <td>{len-no}</td>
          <td>{code}</td>
          <td style={isShowWarrantSelector}>
            <button
              name={code}
              type="button"
              className={classNames("btn btn-sm btn-secondary", btn1Stype)}
              onClick={this.handleAction}>
                {btn1Text}
            </button>
          </td>
          <td> {d.ucode} {uname}</td>
          <td>
            <button
              name={d.ucode}
              data-action={btn2Action}
              type="button"
              className={classNames("btn btn-sm btn-secondary", btn2Stype)}
              onClick={this.handleAction2}>
                {btn2Text}
            </button>
          </td>
          <td> {d.issuer} </td>
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
    
    var style2 = (totalProfitLoss1==0) ? '' : (totalProfitLoss1>0) ? 'font-up' : 'font-down'
    
    return(
      <div className="row">
        <div className="col-12">
          <h6> {text.portfolio} </h6>
          <h4 className={style2}> {text.totalProfitLoss} : {numberWithCommas(parseFloat(totalProfitLoss1).toFixed(2))} </h4>
          <table className="table table-sm table-striped table-light table-portfolio">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="80px" />
              <col span="1" width="80px" />
              <col span="1" width="200px" />
              <col span="1" width="80px" />
              <col span="1" width="50px" />
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
                <th style={isShowWarrantSelector} > </th>
                <th>{text.ucode}</th>
                <th style={isShowWarrantSelector} > </th>
                <th>{text.issuer}</th>
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