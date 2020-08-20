class SignalTable extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  getText(lang) {
    var text = {
      en: {id: 'ID', ucode: 'Underlying', ask: 'ask'},
      sc: {id: 'ID', ucode: '相关资产', ask: '卖牌'},
      tc: {id: 'ID', ucode: '相關資產', ask: '賣牌'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var no = 0, rows1 = []
    for (const [ucode, d] of Object.entries(this.props.data)) {
      no++
      var uname = getUnderlyingName2(ucode)
      //
      var rows2 = []
      for (var d2 of d.detectedlist)
        rows2.push(<td key={'signal_'+no+'_'+d2[0]}>{d2[0]}: {d2[1]}</td>)
      
      //
      rows1.push(
        <tr key={'signal_'+no}>
          <td>{no}</td>
          <td>{ucode} {uname}</td>
          <td>{d.ask}</td>
          {rows2}
        </tr>
      )
    }
    
    return(
      <div className='row'>
        <div className="col-12">
          <h6>Signal</h6>
          <table className="table table-sm table-striped table-light table-signal">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="200px" />
              <col span="1" width="100px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.ucode}</th>
                <th>{text.ask}</th>
                <th>wnt 1</th>
                <th>wnt 2</th>
                <th>wnt 3</th>
                <th>wnt 4</th>
                <th>wnt 5</th>
              </tr>
            </thead>
            <tbody>
              {rows1}
            </tbody>
          </table>
        </div>
      </div>
    )
  }
}