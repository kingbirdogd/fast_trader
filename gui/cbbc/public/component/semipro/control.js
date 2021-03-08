class Control extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
    
    this.handleAction = this.handleAction.bind(this)
    this.handleShutDown = this.handleShutDown.bind(this)
  }
  
  componentDidMount() {

  }
  
  handleAction() {
    
  }
  
  handleShutDown() {
    console.log('handleShutDown')
  }
  
  render() {
    return(

<div className="row control-panel">
  <div className="col-12 col-sm-3 mb-4 mb-sm-2">
  <button name="shutDown" className="btn btn-sm btn-warning" type="button" onClick={this.handleShutDown}><img src="home/images/icon_tick.png" width="15" height="12" /> 停機 (停止所有監控及賣出所有倉位)</button>
  </div>

  <div className="col-12 col-sm-7 mb-2">
  <div className="btn-group mr-2 mr-sm-1">
    <button className="btn btn-sm btn-secondary dropdown-toggle" id="glossary" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">注解</button>
    <div className="dropdown-menu glossary" aria-labelledby="glossary">
      <table width="100%" border="0" cellSpacing="0" cellPadding="0">
        <tbody><tr>
        <th colSpan="5">狀態</th>
        </tr>
        <tr>
        <td><span className="bg_red">股碼</span></td>
        <td align="right">股代碼錯誤</td>
        <td align="right">&nbsp;</td>
        <td><span className="bg_green"></span></td>
        <td align="right">監控中</td>
        </tr>
        <tr>
        <td><span className="bg_red">股價</span></td>
        <td align="right">股價錯誤</td>
        <td>&nbsp;</td>
        <td><span className="bg_orange"></span></td>
        <td align="right">有倉</td>
        </tr>
        <tr>
        <td><span className="bg_red">證碼</span></td>
        <td align="right">證代碼錯誤</td>
        <td width="6%" align="right">&nbsp;</td>
        <td width="15%"><span className="bg_none2"></span></td>
        <td width="32%" align="right">無倉</td>
        </tr>
        <tr>
        <td><span className="bg_red">證價</span></td>
        <td align="right">證價錯誤</td>
        <td>&nbsp;</td>
        <td><span className="bg_yellow"></span></td>
        <td align="right">部分成交</td>
        </tr>
        <tr>
        <td width="15%"><span className="bg_red">證量</span></td>
        <td width="32%" align="right">證量錯誤</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">證單</span></td>
        <td align="right">證訂單號碼錯誤</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">手數</span></td>
        <td align="right">證手數錯誤</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">標的</span></td>
        <td align="right">證與標的不符</td>
        <td align="right">&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">出盤</span></td>
        <td align="right">證訂單已發出</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">動作</span></td>
        <td align="right">動作錯誤</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">系統</span></td>
        <td align="right">系統錯誤</td>
        <td align="right">&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">節流</span></td>
        <td align="right">節流量不足</td>
        <td align="right">&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
        <tr>
        <td><span className="bg_red">金額</span></td>
        <td align="right">購買金額不足</td>
        <td align="right">&nbsp;</td>
        <td>&nbsp;</td>
        <td align="right">&nbsp;</td>
        </tr>
      </tbody></table>
    </div>
  </div>
  |
  <button name="glossary" className="btn btn-sm btn-primary ml-2 ml-sm-1 mr-2 mr-sm-1" type="button">交易</button>
  <button name="position" className="btn btn-sm btn-primary mr-2 mr-sm-1" type="button">倉位</button>
  <button name="order" className="btn btn-sm btn-primary mr-2 mr-sm-1" type="button">當日成交</button>
  | 
  <button name="loadData" className="btn btn-sm btn-secondary ml-2 ml-sm-1 mr-2 mr-sm-1" type="button">載入</button>
  <button name="saveData" className="btn btn-sm btn-secondary mr-2 mr-sm-1" type="button">儲存</button>
  <button name="setting" className="btn btn-sm btn-secondary mr-2 mr-sm-1" type="button">設定</button>
  </div>

  <div className="col-12 col-sm-2">
  <div className="float-md-right">
    用戶狀態:<img className="online" src="/home/images/icon_online.png" />連接中
  </div>
  </div>
</div>

    )
  }
}
